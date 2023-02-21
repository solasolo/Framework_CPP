#pragma once

#ifndef _SF_ATOMIC_H_
#define _SF_ATOMIC_H_

namespace SoulFab::System
{
	class Atomic
	{
	public:
		template<class T> static bool CompareExchange(T volatile* Target, T Exchange, T Comperand);
		template<class T> static long Exchange(T volatile* Target, T Value);

		static long Decrement(long volatile* Target);
		static long Increment(long volatile* Target);
	};

	template<class T> 
	bool Atomic::CompareExchange(T volatile* Target, T Exchange, T Comperand)
	{
		static_assert(sizeof(T) == 8);

		__asm
		{
			mov  rcx, Target;
			mov  rdx, Exchange;
			mov  rax, Comperand;
			lock cmpxchg[rcx], rdx;
		}
	}

	template<class T>
	inline long Atomic::Exchange(T volatile* Target, T Value)
	{
		static_assert(sizeof(T) == 8);

		__asm
		{
			mov  ecx, Target;
			mov  edx, Value;

		label:
			lock cmpxchg[ecx], edx;//¼Ó
			jnz  short label;
		}
	}

	inline long Atomic::Decrement(long volatile* Target)
	{
		__asm
		{
			mov  rcx, Target;
			mov  rax, 0FFFFFFFFh;
			lock xadd[rcx], rax;
			dec  rax;
		}
	}

	inline long Atomic::Increment(long  volatile* Target)
	{
		__asm
		{
			mov  rcx, Target;
			mov  rax, 1;
			lock xadd[rcx], rax;
			inc  rax;
		}
	}
}

#endif