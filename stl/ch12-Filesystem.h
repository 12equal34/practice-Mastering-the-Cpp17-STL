#pragma once

#include <filesystem>

//-----------------------------------------------------------------------------
// [A note about namespaces]
//-----------------------------------------------------------------------------
namespace section1
{
// c++17�� filesystem�� �ϳ��� ������� <filesystem>���� ��� �����Ѵ�.
// �̵��� namespace std::filesystem �ȿ� ��ġ�Ѵ�.
// �׷��� filesystem�� ����� ��, std::filesystem::directory_iteratoró��
// �̸��� ��� �ۼ��ؾ� �ؼ� �����ϴ�. �׷���
// using namespace std::filesystem; �� �ϱ� ����
// using fs = std::filesystem; ���� ����ϱ� �����Ѵ�.
// �ֳ��ϸ� remove(p); ��� �ۼ��ϸ� ��� �Լ����� ��Ȯ���� �ʰ�
// fs::remove(p); �� �ۼ��ϸ� ��Ȯ�ϱ� �����̴�.
// ������ ���ǻ� std::filesystem�� fs�� �ۼ��ϰڴ�.
// ����, a namespace alias fs�� ���� ����ϴ� ������ �ٲ� �� �ִ�.
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