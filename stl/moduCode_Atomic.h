#pragma once

// https://modoocode.com/271

//-----------------------------------------------------------------------------
// ĳ��
//-----------------------------------------------------------------------------
namespace section1
{
// CPU�� ��ǻ�� �޸��� RAM�� ���������� ������ �־
// �޸𸮷κ��� �����͸� �о� ���� ���ؼ��� �ð��� ���� �ɸ���.
// ���� ���,
// �޸𸮷κ��� �����͸� �о���µ� 42 ����Ŭ�� �ɸ���
// ���� ������ 1����Ŭ�� �ɸ���.
// �׷��� �о���� �ð��� ��ٸ��� 42�� ���������� ��ģ��.
//
// CPU Ĩ �ȿ� �ִ� ���� �޸��� ĳ�� Cache�� �����Ѵ�.
// ���������� CPU�� �پ� �ֱ� ������ �б�/���� �ӵ��� �ξ� ������.
//
// ���� ���,
// �ھ�� �������� �ʴ� L1,L2 ĳ�ð� �����Ѵ�.
// L1 ĳ�� : 32KB,			r/w �ӵ� = 4 ����Ŭ
// L2 ĳ�� : 256KB			r/w �ӵ� = 12 ����Ŭ
// �����ϴ� L3 ĳ�� : 8MB		r/w �ӵ� = 36 ����Ŭ
//
// CPU�� Ư���� �ּҿ� �ִ� �����Ϳ� �����Ѵٸ�,
// L1, L2, L3, �޸� ������ �����͸� Ȯ���Ѵ�.
// �����͸� ĳ�ÿ��� �������� Cache hit ��� �Ѵ�.
// �����͸� �޸𸮱��� ���� �������� Cache miss ��� �Ѵ�.
//
// CPU�� �޸𸮸� ������ ĳ�ÿ� �����س��´�.
// ���� ĳ�ð� �� á�ٸ� Ư���� ��Ŀ� ���� ó���Ѵ�.
// Ư���� ����� CPU ���� �ٸ���.
// ��ǥ���� ���� Least Recently Used (LRU) Cache�� ���ο� ĳ�ø� ����ϴ� �����
// �ִ�. (���� ������ ĳ�ø� ���ο� ĳ�÷� ������.)
//
// ���� ���, ĳ�ð� 1KB ũ���̰� LRU ����� ����ϴ� CPU�� ���ؼ� ������ ����.
void cache_miss()
{
    for (int i = 0; i < 10000; ++i) {
        for (int j = 0; j < 10000; ++j) {
            // s += data[j];
        }
    }
    // j = 256�� �Ǿ��� ��, data[0]�� ĳ�ÿ��� �������.
    // ���� �������� data[0]�� ĳ�ÿ� �������� �����Ƿ� �޸𸮿��� �����´�.
}

void cache_hit()
{
    for (int j = 0; j < 10000; j++) {
        for (int i = 0; i < 10000; i++) {
            // s += data[j];
        }
        // ó�� data[0]�� ������ �� �޸𸮿��� �����´�.
        // ���� data[0]�� ��� ĳ�ÿ��� �����´�.
    }
}
}