#pragma once

#include <functional>
#include <sstream>
#include <string>
#include <map>
#include <filesystem>
#include <fstream>

class ReaderBase
{
public:
	ReaderBase(const std::string& filepath);

	using ParseFunc = std::function<void()>;
#define ParsePair(name, f) actions.insert({ name, [=, this] { f(); } })
#define IgnorePair(name) actions.insert({ name, [this] { Ignore(); } })

	void Read();

	std::function<void()> endFunc;

	std::filesystem::path workingDir;
	std::map<std::string, ParseFunc> actions;

protected:
	bool GetChunk(std::string& chunk);
	void Ignore();

	template <class T, std::size_t N>
	struct Vec {
		std::array<T, N> data;

		template <class U>
		operator U() const {
			return construct_from_array<U>(data, std::make_index_sequence<N>());
		}
	};

	template <class T, std::size_t N>
	static Vec<T, N> ReadVec(std::istream& str) {
		Vec<T, N> vec{};

		for (std::size_t i = 0; i < N; ++i)
			str >> vec.data[i];

		return vec;
	}

	using Vec2f = Vec<float, 2>;
	using Vec3f = Vec<float, 3>;
	using Vec4f = Vec<float, 4>;

	std::ifstream m_fileStr;
private:
	void ParseTokens(const std::string& chunk);
};

