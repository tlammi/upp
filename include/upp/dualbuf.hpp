#pragma once

#include <utility>

namespace upp{

/**
 * Utility for double buffering
 *
 * Back buffer can be read while front buffer is manipulated
 */
template<class T>
class DualBuf{
public:

	DualBuf(){}

	/// Access front buffer
	T* front() noexcept {
		return front_;
	}

	/// Access back buffer
	const T* back() const noexcept {
		return back_;
	}

	/// Swap buffers
	///
	/// Only pointer are swapped, no
	/// actual relocations are performed
	void swap() noexcept {
		std::swap(front_, back_);
	}
private:
	T buf0_{};
	T buf1_{};
	T* front_{&buf0_};
	T* back_{&buf1_};
};

}
