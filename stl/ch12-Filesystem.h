#pragma once

#include <filesystem>

//-----------------------------------------------------------------------------
// [A note about namespaces]
//-----------------------------------------------------------------------------
namespace section1
{
// c++17의 filesystem은 하나의 헤더파일 <filesystem>에서 모두 제공한다.
// 이들은 namespace std::filesystem 안에 위치한다.
// 그래서 filesystem을 사용할 때, std::filesystem::directory_iterator처럼
// 이름을 길게 작성해야 해서 불편하다. 그래서
// using namespace std::filesystem; 을 하기 보다
// using fs = std::filesystem; 으로 사용하길 권장한다.
// 왜냐하면 remove(p); 라고 작성하면 어떠한 함수인지 명확하지 않고
// fs::remove(p); 라 작성하면 명확하기 때문이다.
// 앞으로 편의상 std::filesystem을 fs라 작성하겠다.
// 또한, a namespace alias fs를 통해 사용하는 구현을 바꿀 수 있다.
// #if USE_CXX17
//		#include <filesystem>
//		namespace fs = std::filesystem;
// #elif USE_FILESYSTEM_TS
//		#include <experimental/filesystem>
//		namespace fs = std::experimental::filesystem;
// #elif USE_BOOST
//		#include <boost/filesystem.hpp>
//		namespace fs = boost::filesystem;
// #endif
}