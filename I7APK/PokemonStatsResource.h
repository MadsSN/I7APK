#pragma once
#include <iostream>
#include <memory_resource>

// based on implementation from class slides
class PokemonStatsResource : public std::pmr::memory_resource {
public:
	// is this name okay, example have them with different names
	PokemonStatsResource(std::pmr::memory_resource* parent = std::pmr::get_default_resource()) : parent_(parent) {}

	void printStats() {
		std::cout << "Currently allocated bytes: " << bytesAllocated_ << std::endl
			<< "High watermark of allocated bytes: " << highWatermarkBytesAllocated_ << std::endl;
			/*<< "Currently allocated pokemon: " << bytesAllocated_ / sizeof(Pokemon) << std::endl
			<< "High watermark of allocated pokemon: " << highWatermarkBytesAllocated_ / sizeof(Pokemon) << std::endl;*/
	}

private:
	std::pmr::memory_resource* parent_;
	int32_t bytesAllocated_ {};
	int32_t highWatermarkBytesAllocated_ {};

	void* do_allocate(size_t bytes, size_t align) override {
		bytesAllocated_ += bytes;

		if (bytesAllocated_ > highWatermarkBytesAllocated_) {
			highWatermarkBytesAllocated_ = bytesAllocated_;
		}

		return parent_->allocate(bytes, align);
	}

	void do_deallocate(void* p, size_t bytes, size_t align) override {
		bytesAllocated_ -= bytes;

		parent_->deallocate(p, bytes, align);
	}

	bool do_is_equal(std::pmr::memory_resource const & other) const noexcept override {
		return parent_->is_equal(other);
	}
};

