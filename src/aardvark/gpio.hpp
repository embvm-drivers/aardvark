// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef AARDVARK_GPIO_HPP_
#define AARDVARK_GPIO_HPP_

#include "base.hpp"
#include <driver/gpio.hpp>

namespace embdrv
{
/** Aardvark GPIO driver
 *
 * Create a GPIO instance using the Aardvark adapter.
 *
 * This driver requires an aardvarkAdapter to work.
 *
 * @precondition The aardvark adapter must be configured with aardvarkMode::GpioI2C,
 * 	aardvarkMode::SpiGpio, or aardvarkMode::GpioOnly.
 *
 * @code
 * embdrv::aardvarkAdapter aardvark{embdrv::aardvarkMode::GpioI2C};
 * embdrv::aardvarkGPIO gpio5{aardvark, 5};
 * @endcode
 *
 * @ingroup AardvarkDrivers
 */
class aardvarkGPIO final : public embvm::gpio::base
{
  public:
	/** Construct an Aardvark GPIO
	 *
	 * @param [in] master The aardvarkAdapter instance associated with this GPIO pin.
	 * @param [in] pin The integral representation of Aardvark pin, between (0..5)
	 */
	explicit aardvarkGPIO(aardvarkAdapter& master, uint8_t pin) noexcept;

	/** Construct an Aardvark GPIO
	 *
	 * @param [in] master The aardvarkAdapter instance associated with this GPIO pin.
	 * @param [in] pin The integral representation of Aardvark pin, between (0..5)
	 * @param [in] mode The GPIO mode to use with this pin.
	 */
	explicit aardvarkGPIO(aardvarkAdapter& master, uint8_t pin, embvm::gpio::mode mode) noexcept;

	/// Default destructor
	~aardvarkGPIO() noexcept = default;

	void set(bool v) noexcept final;
	bool get() noexcept final;
	void toggle() noexcept final;
	void setMode(embvm::gpio::mode mode) noexcept final;
	embvm::gpio::mode mode() noexcept final;

  private:
	void start_() noexcept final;
	void stop_() noexcept final;

  private:
	/// The aardvarkAdapter instance associated with this GPIO output.
	aardvarkAdapter& master_;

	/// Integral pin number for this GPIO instance.
	const uint8_t pin_;

	/// Currently configured GPIO mode
	embvm::gpio::mode mode_;

	/// Currently configured output state (high/low)
	/// A toggle API isn't provided, so we manually track this for the toggle() implementation.
	bool output_ = false;
};

} // namespace embdrv

#endif // AARDVARK_GPIO_HPP_
