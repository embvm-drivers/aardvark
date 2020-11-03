// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef AARDVARK_BASE_HPP_
#define AARDVARK_BASE_HPP_

#include <array>
#include <atomic>
#include <driver/driver.hpp>
#include <driver/gpio.hpp>
#include <mutex>

namespace embdrv
{
/// @addtogroup AardvarkDrivers
/// @{

/// The number of IO pins supported by the AARDVARK sensor
inline constexpr size_t AARDVARK_IO_COUNT = 6;

/// Aardvark master operational modes
enum class aardvarkMode
{
	/// Aardvark Device used for GPIO.
	/// Enables the full array of GPIO IOs for use.
	GpioOnly = 0x0,
	/// Aardvark device used for SPI + GPIO.
	/// The Non-SPI pins can be used in GPIO mode.
	SpiGpio = 0x01,
	/// Aardvark device used for I2C + GPIO.
	/// The non-I2C pins can be used in GPIO mode.
	GpioI2C = 0x02,
	/// Aardvark Device used for I2C + SPI.
	/// GPIO use is not supported
	SpiI2C = 0x03,
	/// Query the current mode.
	Query = 0x80
};

/** Driver to control the Aardvark Adapter
 *
 * This class must always be declared for use with Aardvark drivers. The aardvarkAdapter
 * is used to set the operational mode and configure the device hardware.
 *
 * The aardvarkAdapter instance is then passed to the aardvark drivers:
 *
 * @code
 * embdrv::aardvarkAdapter aardvark{embdrv::aardvarkMode::GpioI2C};
 * embdrv::aardvarkI2CMaster i2c0{aardvark};
 * embdrv::aardvarkGPIOOutput<5> gpio5{aardvark};
 * embdrv::aardvarkGPIOInput<4> gpio4{aardvark};
 * embdrv::aardvarkGPIOInput<3> gpio3{aardvark};
 * @endcode
 */
class aardvarkAdapter final : public embvm::DriverBase
{
  public:
	/** Create an aardvark Adapter controller
	 *
	 * @param m The aardvark adapter mode.
	 * @param USBPort The id of the USB port the aardvarkAdapter is connected to.
	 */
	explicit aardvarkAdapter(aardvarkMode m = aardvarkMode::SpiI2C, uint8_t USBPort = 0) noexcept
		: embvm::DriverBase(embvm::DriverType::Undefined), port_(USBPort), mode_(m)
	{
	}

	/// Default destructor
	~aardvarkAdapter() noexcept = default;

	/// Change the Aardvark Adapter's operational mode.
	/// @param m The desired aardvark operational mode.
	/// @returns the configured aardvark operational mode.
	aardvarkMode mode(aardvarkMode m) noexcept;

	/// Get the current mode setting
	/// This function does not query the device, it returns the cached mode.
	/// @returns the configured aardvark operational mode.
	aardvarkMode mode() const noexcept
	{
		return mode_;
	}

	/// Get the handle for the Aardvark Master
	/// The handle is used by related drivers to work with the Aardvark library APIs.
	/// @returns the Aardvark master handle.
	int handle() const noexcept
	{
		return handle_;
	}

	/// Lock the Aardvark master - used by other Aardvark driver types to ensure exclusion
	/// @post The aardvarkAdapter is locked for the client's exclusive use.
	void lock() noexcept
	{
		lock_.lock();
	}

	/// Unlock the Aardvark Master
	/// @pre The aardvarkAdapter is locked
	/// @post The aardvarkAdapter is unlocked
	void unlock() noexcept
	{
		lock_.unlock();
	}

	/// Query the current i2c pullup setting
	/// @returns true if I2C pullups are enabled, false if disabled.
	bool i2cPullups() noexcept;

	/// Enable/Disable the i2c pullup setting
	/// @param en True enables I2C pullups, false disables them.
	/// @returns true if I2C pullups are enabled, false if disabled.
	bool i2cPullups(bool en) noexcept;

	/// Query the current 5V target power setting
	/// @returns true if 5V target power is enabled, false if disabled.
	bool targetPower() noexcept;

	/// Enable/Disable 5V target power output
	/// @param en True enables 5V target power output, false disables it.
	/// @returns true if 5V target power is enabled, false if disabled.
	bool targetPower(bool en) noexcept;

	/// Set a pullup for GPIO
	/// @param id The GPIO Id to enable the pullup for
	/// @param en True enables a pullup, false disables it.
	void pullup(uint8_t id, bool en) noexcept;

	/// Configure a pin as a GPIO input or output
	///
	/// @precondition Aardvark base class is started
	/// @precondition pin is an integer < AARDVARK_IO_COUNT
	///
	/// @param [in] pin The pin to set
	/// @param [in] m The GPIO mode to set the pin to (input/output)
	void setGPIOMode(uint8_t pin, embvm::gpio::mode m) noexcept;

	/// Set GPIO output
	///
	/// @precondition Aardvark base class is started
	/// @precondition pin is an integer < AARDVARK_IO_COUNT
	/// @postcondition GPIO mode for the pin is set to output
	///
	/// @param [in] pin The pin to set
	/// @param [in] v The pin output state; high = true, low = false
	void setGPIOOutput(uint8_t pin, bool v) noexcept;

	/// Read the current GPIO state
	///
	/// @preconditon Aardvark base class is started
	/// @precondition pin is an integer < AARDVARK_IO_COUNT
	/// @precondition pin is set to input
	///
	/// @param [in] pin The pin to read
	/// @returns The current state of the pin
	bool readGPIO(uint8_t pin) noexcept;

  private:
	void start_() noexcept final;
	void stop_() noexcept final;

  private:
	/// The aardvark adapter lock.
	std::mutex lock_{};

	/// The USB port the adapter is connected to.
	uint8_t port_;

	/// Mask representing the pullups currently enabled.
	uint8_t pullup_mask_ = 0;

	/// The handle for the aardvark Adapter (provided by the aardvark API).
	int handle_ = 0;

	/// The currently configured Aardvark Adapter mode.
	aardvarkMode mode_;

	/// Reference count for the number of times this driver has been started.
	/// Since multiple client drivers can be created, we don't want to stop the
	/// aardvarkAdapter base until all client drivers have been stopped.
	std::atomic<int> started_refcnt_ = 0;

	/// Bitmask for GPIO input/output directions
	uint8_t direction_mask_ = 0;

	/// Bitmask for GPIO output settings
	uint8_t output_mask_ = 0;
};

/// @}

} // namespace embdrv

#endif // AARDVARK_BASE_HPP_
