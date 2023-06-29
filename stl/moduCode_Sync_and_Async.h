#pragma once

#include <string>
#include <future>
#include <iostream>
#include <thread>

//-----------------------------------------------------------------------------
// ����� �񵿱� ����
//-----------------------------------------------------------------------------
namespace section1
{
// SSD�� �ƴ� �ϵ� ��ũ���� ������ �о���� �ð��� ����� ���� �ɸ���.
// �뷫 4.17 �и��ʰ� �ɸ��� ������ �����͸� �о���µ� 50�����ʿ� ����
// 8���� ���� ������.
// string txt    = read("a.txt");              // 5ms �ҿ� (�ϵ��ũ �б�)
// string result = do_something_with_txt(txt); // 5ms �ҿ� (cpu ����)
// do_other_computation(); // 5ms �ҿ� (cpu ����)
// �� 15ms�� �ҿ�ȴ�. 
// read �Լ��� ������ �ϵ� ��ũ���� �о���� ����, CPU�� ��ٸ��� ������ ��ȿ�����̴�.
// �̷��� ���������� �����ϴ� �۾��� ������ (synchronous)���� �����Ѵٰ� �Ѵ�.
// �̷��� ��쿡 read �Լ��� �ϵ� ��ũ���� �����͸� �о���� ���ȿ� CPU��
// do_other_computation�� �۾��ϴ� ���� ȿ������ ���̴�.

// C++�� ������� ������ ���� ������ �� �ִ�.
//		void file_read(string* result) {
//			string txt = read("a.txt");  // 5ms �ҿ� (�ϵ��ũ �б�)
//			*result = do_something_with_txt(txt); // 5ms �ҿ� (cpu ����)
//		}
//		int main()
//		{
//			string result;
//			thread t(file_read, &result);
//			do_other_computation();  // 5ms �ҿ� (cpu ����)
//		
//			t.join();
//		}
// �� 10ms�� �ҿ�ȴ�.
// �̷��� ���α׷��� ������ ���� ������ �������� ���ÿ� ����Ǿ�
// �񵿱��� (asynchronous) �����̶�� �θ���.
// c++11 ���� ǥ�� ���̺귯���� ���� ������ �񵿱��� ������ �� �� �ִ�.
}


//-----------------------------------------------------------------------------
// std::promise �� std::future
//-----------------------------------------------------------------------------
namespace section2
{
// �񵿱��� �������� ������ ���� �۾��� ���Ѵ�.
// "������ T�� ����ؼ� �񵿱������� T�� �۾��� �Ϸ�Ǹ� ���ϰ��� �޴´�."
// �̸� �ٽ� ���ϸ�,
// "future�� ������ T�� ��ӵ� �����͸� ���� �ְڴٴ� promise�� �Ѵ�."
void worker(std::promise<std::string>& p)
{
	p.set_value("some data");
}
void test()
{
	std::promise<std::string> p;
	std::future<std::string> data = p.get_future();
	std::thread t(worker, std::ref(p));

	data.wait();
	
	std::cout << "���� ������: " << data.get() << std::endl;

	t.join();
}
}


