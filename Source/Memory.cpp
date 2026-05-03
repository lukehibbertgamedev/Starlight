#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "Memory.h"
#include "../Include/Memory.h"
#include "../Include/Profiler.h"

namespace Starlight
{
	void* operator new(std::size_t size)
	{
		SystemZoneScoped;

		// Store the size of the allocation at the beginning of the block.
		size += sizeof(std::size_t);

		void* ptr = malloc(size);
		*(std::size_t*)ptr = size;

		// Track the allocation with Tracy.
		TracyAlloc(ptr, size);

		// Internal starlight memory tracking.
		Memory& instance = Memory::Instance();
		instance.SetAllocatedBytes(instance.GetAllocatedBytes() + size);
		instance.AddAllocation();

		// Return and offset to the next block of memory.
		return ((size_t*)ptr) + 1;
	}

	void operator delete(void* ptr) noexcept
	{
		SystemZoneScoped;

		if (!ptr)
		{
			return;
		}

		// Free the entire block of memory, including the size information.
		std::size_t size = *(((std::size_t*)ptr) - 1);
		ptr = (void*)(((std::size_t*)ptr) - 1);

		// Free via Tracy.
		TracyFree(ptr);

		// Internal starlight memory tracking.
		Memory& instance = Memory::Instance();
		instance.SetFreedBytes(instance.GetFreedBytes() + size);
		instance.RemoveAllocation();

		free(ptr);
		ptr = nullptr;
	}



	Starlight::Memory::Memory()
	{
		StarlightZoneScoped;

		allocationCount = 0u;
		allocatedBytes = 0u;
		freedBytes = 0u;

	}

	Memory& Starlight::Memory::Instance()
	{
		StarlightZoneScoped;

		static Memory instance;
		return instance;
	}

	const uint64_t Starlight::Memory::GetCurrentlyUsedBytes() const
	{
		StarlightZoneScoped;

		return allocatedBytes - freedBytes;
	}

	const uint64_t Starlight::Memory::GetAllocationCount() const
	{
		StarlightZoneScoped;

		return allocationCount;
	}

	const uint64_t Starlight::Memory::GetAllocatedBytes() const
	{
		StarlightZoneScoped;

		return allocatedBytes;
	}

	const uint64_t Starlight::Memory::GetFreedBytes() const
	{
		StarlightZoneScoped;

		return freedBytes;
	}

	void Starlight::Memory::SetAllocatedBytes(const uint64_t allocated)
	{
		StarlightZoneScoped;

		std::lock_guard<std::mutex> guard(lock);
		allocatedBytes = allocated;
	}

	void Starlight::Memory::SetFreedBytes(const uint64_t freed)
	{
		StarlightZoneScoped;

		std::lock_guard<std::mutex> guard(lock);
		freedBytes = freed;
	}

	void Starlight::Memory::AddAllocation()
	{
		StarlightZoneScoped;

		std::lock_guard<std::mutex> guard(lock);
		++allocationCount;
	}

	void Starlight::Memory::RemoveAllocation()
	{
		StarlightZoneScoped;

		std::lock_guard<std::mutex> guard(lock);
		--allocationCount;
	}
}