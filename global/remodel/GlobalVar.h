#pragma once

template <typename T, int offset>
class GlobalVar {
public:
	GlobalVar& operator=(const T& value)
	{
		*reinterpret_cast<T*>(offset) = value;
		return *this;
	}

    T* operator->() const { return reinterpret_cast<T*>(offset); }
	operator T&() const { return *reinterpret_cast<T*>(offset); }
	operator T*() const { return reinterpret_cast<T*>(offset); }

};
