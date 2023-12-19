#pragma once
#include "../dependencies.hpp"

namespace Clock {
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> duration;
	namespace chrono = std::chrono;


	class Timer {
		chrono::system_clock::time_point m_start, m_end;
		ms m_result;

	public:
		Timer() {
			m_start = chrono::system_clock::now();
		}

		~Timer() {
			m_end = chrono::system_clock::now();
			m_result = chrono::duration_cast<ms>(m_end - m_start);

			std::cout << m_result.count() << "\n";
		}
	};
}