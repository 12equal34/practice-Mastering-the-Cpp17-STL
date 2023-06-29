#pragma once

#include <string>
#include <future>
#include <iostream>
#include <thread>

//-----------------------------------------------------------------------------
// 동기와 비동기 실행
//-----------------------------------------------------------------------------
namespace section1
{
// SSD가 아닌 하드 디스크에서 파일을 읽어오면 시간이 상당히 오래 걸린다.
// 대략 4.17 밀리초가 걸리면 램에서 데이터를 읽어오는데 50나노초에 비해
// 8만배 정도 느리다.
// string txt    = read("a.txt");              // 5ms 소요 (하드디스크 읽기)
// string result = do_something_with_txt(txt); // 5ms 소요 (cpu 연산)
// do_other_computation(); // 5ms 소요 (cpu 연산)
// 총 15ms가 소요된다. 
// read 함수가 파일을 하드 디스크에서 읽어오는 동안, CPU가 기다리기 때문에 비효율적이다.
// 이렇게 순차적으로 실행하는 작업을 동기적 (synchronous)으로 실행한다고 한다.
// 이러한 경우에 read 함수가 하드 디스크에서 데이터를 읽어오는 동안에 CPU가
// do_other_computation을 작업하는 것이 효율적일 것이다.

// C++의 쓰레드로 다음과 같이 구현할 수 있다.
//		void file_read(string* result) {
//			string txt = read("a.txt");  // 5ms 소요 (하드디스크 읽기)
//			*result = do_something_with_txt(txt); // 5ms 소요 (cpu 연산)
//		}
//		int main()
//		{
//			string result;
//			thread t(file_read, &result);
//			do_other_computation();  // 5ms 소요 (cpu 연산)
//		
//			t.join();
//		}
// 총 10ms가 소요된다.
// 이러한 프로그램의 실행은 여러 갈래로 나뉘어져 동시에 진행되어
// 비동기적 (asynchronous) 실행이라고 부른다.
// c++11 부터 표준 라이브러리를 통해 간단히 비동기적 실행을 할 수 있다.
}


//-----------------------------------------------------------------------------
// std::promise 와 std::future
//-----------------------------------------------------------------------------
namespace section2
{
// 비동기적 실행으로 다음과 같은 작업을 원한다.
// "쓰레드 T를 사용해서 비동기적으로 T의 작업이 완료되면 리턴값을 받는다."
// 이를 다시 말하면,
// "future에 쓰레드 T가 약속된 데이터를 돌려 주겠다는 promise을 한다."
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
	
	std::cout << "받은 데이터: " << data.get() << std::endl;

	t.join();
}
}


