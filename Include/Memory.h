#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <mutex>
#include <cstdint>
#include <string>

namespace Starlight
{
	class Memory
	{
	public:

		Memory();

		Memory(const Memory&) = delete;
		Memory(Memory&&) = delete;

		static Memory& Instance();

		const uint64_t GetCurrentlyUsedBytes() const;
		const uint64_t GetAllocationCount() const;
		const uint64_t GetAllocatedBytes() const;
		const uint64_t GetFreedBytes() const;

		void SetAllocatedBytes(const uint64_t allocated);
		void SetFreedBytes(const uint64_t freed);

		void AddAllocation();
		void RemoveAllocation();

	private:

		std::mutex lock;

		uint64_t allocationCount;
		uint64_t allocatedBytes;
		uint64_t freedBytes;


	};
}

#endif
