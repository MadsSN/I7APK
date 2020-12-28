#pragma once
#include <iostream>
#include <memory_resource>
#include "Pokemon.h"

// based on implementation from class slides
class PokemonStatsResource : public std::pmr::memory_resource {
public:
	PokemonStatsResource(std::pmr::memory_resource* parent = std::pmr::get_default_resource()) : parent_(parent) {}

	void printStats() {
		std::cout << "\n~~ PokeDex allocated pokemon data ~~" << std::endl
			<< "Currently allocated Pokemon bytes: " << bytesAllocated_ << std::endl
			<< "High watermark of allocated Pokemon bytes: " << highWatermarkBytesAllocated_ << std::endl;
		/*
			<< "Currently allocated pokemon: " << (int)(bytesAllocated_ / sizeof(Pokemon))-1 << std::endl
			<< "High watermark of allocated pokemon: " << (int)(highWatermarkBytesAllocated_ / sizeof(Pokemon))-1 << std::endl;
		*/
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

