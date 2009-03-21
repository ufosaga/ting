/* The MIT License:

Copyright (c) 2009 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE. */

// ting 0.3
// Homepage: http://code.google.com/p/ting
// Author: Ivan Gagis <igagis@gmail.com>

// File description:
//	Singleton base class (template)

#ifndef M_Singleton_hpp
#define M_Singleton_hpp

#include "Exc.hpp"
#include "debug.hpp"

namespace ting{

template <class T> class Singleton{
	
	inline static T*& StaticMemoryBlock(){
		static T* instance = 0;
		return instance;
	};

protected://use only as a base class
	Singleton(){
		if(Singleton::StaticMemoryBlock() != 0)
			throw ting::Exc("Singleton::Singleton(): instance is already created");

		Singleton::StaticMemoryBlock() = static_cast<T*>(this);
	};

public:
	inline static bool IsCreated(){
		return Singleton::StaticMemoryBlock() != 0;
	};

	inline static T& Inst(){
		ASSERT(Singleton::IsCreated())
		return *(Singleton::StaticMemoryBlock());
	};

	~Singleton(){
		ASSERT(Singleton::StaticMemoryBlock() == static_cast<T*>(this))
		Singleton::StaticMemoryBlock() = 0;
	};
};

};//~namespace igagis
#endif//~once