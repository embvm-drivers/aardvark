// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#include "gpio.hpp"
#include "vendor/aardvark.h"
#include <cassert>

using namespace embvm;
using namespace embdrv;

aardvarkGPIO::aardvarkGPIO(aardvarkAdapter& master, uint8_t pin) noexcept
	: master_(master), pin_(pin), mode_(gpio::mode::input)
{
	assert(pin < AARDVARK_IO_COUNT);
}

aardvarkGPIO::aardvarkGPIO(aardvarkAdapter& master, uint8_t pin, gpio::mode mode) noexcept
	: master_(master), pin_(pin), mode_(mode)
{
	assert(pin < AARDVARK_IO_COUNT);
}

void aardvarkGPIO::set(bool v) noexcept
{
	master_.setGPIOOutput(pin_, v);
	output_ = v;
}

bool aardvarkGPIO::get() noexcept
{
	return master_.readGPIO(pin_);
}

void aardvarkGPIO::toggle() noexcept
{
	set(!output_);
}

void aardvarkGPIO::setMode(embvm::gpio::mode mode) noexcept
{
	master_.setGPIOMode(pin_, mode);
}

embvm::gpio::mode aardvarkGPIO::mode() noexcept
{
	return mode_;
}

void aardvarkGPIO::aardvarkGPIO::start_() noexcept
{
	master_.start();
	setMode(mode_);
}

void aardvarkGPIO::aardvarkGPIO::stop_() noexcept
{
	setMode(gpio::mode::input);
	master_.stop();
}
