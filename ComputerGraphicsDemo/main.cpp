// Dear ImGui: standalone example application for Win32 + OpenGL 3
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// This is provided for completeness, however it is strogly recommended you use OpenGL with SDL or GLFW.

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/GL.h>
#include <tchar.h>

#include <array>

#include "Renderer.h"
#include "Scene.h"
#include "Viewport.h"
#include "Loader.h"
#include "GUI/ScenePropertyWindow.h"
#include "GUI/MeshNodePropertyWindow.h"

namespace dx = DirectX;

Viewport viewport(1280, 800);
Loader loader;

// Data stored per platform window
struct WGL_WindowData { HDC hDC; };

// Data
static HGLRC            g_hRC;
static WGL_WindowData   g_MainWindow;


// Forward declarations of helper functions
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data);
void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ProcEvents();

Matrix4x4 Orbit(float prevX, float prevY, float x, float y, const Viewport& vp, const Matrix4x4& lookAt, Point rotationPoint);
Matrix4x4 Pan(float prevX, float prevY, float x, float y, const Viewport& vp, Point pointOnPlane);

// Main code
int main(int, char**)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui Win32+OpenGL3 Example", WS_OVERLAPPEDWINDOW, 100, 100, viewport.width, viewport.height, NULL, NULL, wc.hInstance, NULL);
    PostMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);

    // Initialize OpenGL
    if (!CreateDeviceWGL(hwnd, &g_MainWindow))
    {
        CleanupDeviceWGL(hwnd, &g_MainWindow);
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    wglMakeCurrent(g_MainWindow.hDC, g_hRC);

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_InitForOpenGL(hwnd);
    ImGui_ImplOpenGL3_Init();

    Renderer renderer;

    loader.SetWorkingDir("C:\\Users\\turon\\Downloads\\FinalBaseMesh\\Cube");

    ScenePropertyWindow sceneProperty;
    MeshNodePropertyWindow nodeProperty;
    sceneProperty.SetLoader(&loader);
    loader.index = 1;


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        renderer.BeginFrame(viewport.width, viewport.height);
        ImGui::NewFrame();

        ImGui::Begin("Info");
        ImGui::Text("FPS: %f", 1.f / io.DeltaTime);
        ImGui::Text("Delta time: %f", io.DeltaTime);
        ImGui::End();

        // Rendering

        loader.Update();

        ProcEvents();

        sceneProperty.Render();

        auto& scene = loader.GetScene();
        auto selectedNode = scene.GetSelectedNode();
        if (selectedNode) {
            nodeProperty.SetMeshNode(selectedNode);
            nodeProperty.Render();
        }

        scene.RenderScene(renderer, viewport.ProjectionMatrix());

        ImGui::Render();

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Present
        ::SwapBuffers(g_MainWindow.hDC);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceWGL(hwnd, &g_MainWindow);
    wglDeleteContext(g_hRC);
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

void ProcEvents() {
    auto& scene = loader.GetScene();
    auto& io = ImGui::GetIO();
    const float cameraDelta = 2.f * io.DeltaTime;
    const float zoomDelta = 0.3f;
    auto& camera = scene.GetCamera();

    if (ImGui::IsKeyDown(ImGuiKey_A))
        camera.Translate(-cameraDelta * camera.CalcRight());
    else if (ImGui::IsKeyDown(ImGuiKey_D))
        camera.Translate(cameraDelta * camera.CalcRight());
    
    else if (ImGui::IsKeyDown(ImGuiKey_W))
        camera.Translate(cameraDelta * camera.CalcDir());
    else if (ImGui::IsKeyDown(ImGuiKey_S))
        camera.Translate(-cameraDelta * camera.CalcDir());


    if (io.WantCaptureMouse)
        return;

    if (io.MouseWheel != 0)
        camera.Zoom(io.MouseWheel * zoomDelta);

    auto selectedNode = scene.GetSelectedNode();

    if ((io.MouseDelta.x != 0.f) || (io.MouseDelta.y != 0.f)) {
        const ImVec2& delta = io.MouseDelta;
        const ImVec2& pos = io.MousePos;
        const ImVec2 prevPos{ pos.x - delta.x, pos.y - delta.y };

        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
            if (io.KeyShift && selectedNode) {
                const auto& center = selectedNode->GetBoundingBox().Center;
                auto matrix = Orbit(pos.x, pos.y, prevPos.x, prevPos.y, viewport, camera.LookAt(), dx::XMVectorSet(center.x, center.y, center.z, 1.0f));
                selectedNode->Transform(matrix);
            }
            else {
                camera.Transform(Orbit(prevPos.x, prevPos.y, pos.x, pos.y, viewport, camera.LookAt(), camera.m_target));
            }
        else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            if (io.KeyShift && selectedNode) {
                const auto& center = selectedNode->GetBoundingBox().Center;
                selectedNode->Transform(Pan(pos.x, pos.y, prevPos.x, prevPos.y, viewport, dx::XMVectorSet(center.x, center.y, center.z, 1.0f)));
            }
            else {   
                camera.Transform(Pan(prevPos.x, prevPos.y, pos.x, pos.y, viewport, camera.m_target));
            }
        }
    }

    auto intersected = scene.FindIntersected(camera.CalcCursorRay(io.MousePos.x, io.MousePos.y, viewport));

    if (!intersected.empty() && !io.WantCaptureMouse) {
        const auto& [dist, closestNode] = *intersected.begin();
        scene.HighlightNode(closestNode);

        if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            scene.SelectNode(io.KeyCtrl ? nullptr : closestNode);
    }
}

Matrix4x4 Orbit(float prevX, float prevY, float x, float y, const Viewport& vp, const Matrix4x4& lookAt, Point rotationPoint)
{
    auto calcVec = [=](float x, float y) {
        const float xCenter = vp.width / 2.f;
        const float yCenter = vp.height / 2.f;
        x = 2 * (x - xCenter) / vp.width * vp.AspectRatio();
        y = 2 * (y - yCenter) / vp.height;


        const float sq = x * x + y * y;

        float z = 0.0f;
        if (sq < 1.0f)
            z = sqrtf(1.0f - sq);

        return dx::XMVectorSet(x, -y, z, 0);
    };

    auto prevVec = calcVec(prevX, prevY);
    auto nextVec = calcVec(x, y);

    auto vangle = dx::XMVector3AngleBetweenVectors(prevVec, nextVec);
    float angle{};
    dx::XMStoreFloat(&angle, vangle);

    auto vreverseLookAt = XMMatrixInverse(nullptr, lookAt);
    auto vaxis = XMVector3TransformNormal(dx::XMVector3Cross(prevVec, nextVec), vreverseLookAt);
    auto vquat = dx::XMQuaternionRotationAxis(vaxis, -angle);

    auto T = dx::XMMatrixTranslationFromVector(rotationPoint);
    auto IT = dx::XMMatrixInverse(nullptr, T);

    return IT * dx::XMMatrixRotationQuaternion(vquat) * T;
}

Matrix4x4 Pan(float prevX, float prevY, float x, float y, const Viewport& vp, Point pointOnPlane)
{
    auto& camera = loader.GetScene().GetCamera();
    auto dir = camera.CalcDir();

    auto prevRay = camera.CalcCursorRay(prevX, prevY, vp);
    auto newRay = camera.CalcCursorRay(x, y, vp);
    auto targetPlane = dx::XMPlaneFromPointNormal(pointOnPlane, dir);
    auto prevPos = dx::XMPlaneIntersectLine(targetPlane, prevRay.origin, prevRay.origin + prevRay.dir * (vp.farZ - vp.nearZ));
    auto newPos = dx::XMPlaneIntersectLine(targetPlane, newRay.origin, newRay.origin + newRay.dir * (vp.farZ - vp.nearZ));

    return dx::XMMatrixTranslationFromVector(prevPos - newPos);
}

// Helper functions
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!g_hRC)
        g_hRC = wglCreateContext(data->hDC);
    return true;
}

void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    wglMakeCurrent(NULL, NULL);
    ::ReleaseDC(hWnd, data->hDC);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            viewport.width = LOWORD(lParam);
            viewport.height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
