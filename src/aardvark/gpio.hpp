#ifndef AARDVARK_GPIO_HPP_
#define AARDVARK_GPIO_HPP_

#include "base.hpp"
#include "vendor/aardvark.h" // TODO: refactor so this isn't necessary
#include <cassert>
#include <driver/gpio.hpp>

namespace embdrv
{
/** Aardvark GPIO Input driver
 *
 * Create a GPIO input driver using the Aardvark adapter.
 *
 * This driver requires an aardvarkAdapter to work. The aardvark adapter must be
 * configured with aardvarkMode::GpioI2C, aardvarkMode::SpiGpio, or aardvarkMode::GpioOnly.
 *
 * @code
 * embdrv::aardvarkAdapter aardvark{embdrv::aardvarkMode::GpioI2C};
 * embdrv::aardvarkGPIOInput<4> gpio4{aardvark};
 * embdrv::aardvarkGPIOInput<3> gpio3{aardvark};
 * @endcode
 *
 * @tparam TPinID The aardvark pin ID to use as a GPIO Input. Must be less than AARDVARK_IO_COUNT.
 * @tparam TPull The pull setting to use with this GPIO.
 * @ingroup AardvarkDrivers
 */
template<uint8_t TPinID, embvm::gpio::pull TPull = embvm::gpio::pull::none>
class aardvarkGPIOInput final : public embvm::gpio::input<TPull>
{
  public:
	/** Construct a generic GPIO input
	 *
	 * @param master The aardvarkAdapter instance associated with this GPIO pin.
	 */
	explicit aardvarkGPIOInput(aardvarkAdapter& master) noexcept
		: embvm::gpio::input<TPull>("Aardvark GPIO"), master_(master)
	{
		assert(TPinID < AARDVARK_IO_COUNT && "Invalid ID");
	}

	/** Construct a named GPIO Input
	 *
	 * @param master The aardvarkAdapter instance associated with this GPIO pin.
	 * @param name The name of the GPIO pin
	 */
	explicit aardvarkGPIOInput(aardvarkAdapter& master, const char* name) noexcept
		: embvm::gpio::input<TPull>(name), master_(master)
	{
		assert(TPinID < AARDVARK_IO_COUNT && "Invalid ID");
	}

	/// Default Destructor
	~aardvarkGPIOInput() noexcept final = default;

	bool get() noexcept final
	{
		bool set = false;

		if(embvm::DriverBase::started())
		{
			master_.lock();
			set = aa_gpio_get(master_.handle()) & aardvarkIO[TPinID];
			master_.unlock();
		}

		return set;
	}

  private:
	void start_() noexcept final
	{
		master_.start();

		if(master_.mode() != aardvarkMode::GpioI2C)
		{
			master_.i2cPullups(false);
		}

		pull_(TPull);
	}

	void stop_() noexcept final
	{
		master_.stop();
	}

	embvm::gpio::pull pull_(embvm::gpio::pull p) noexcept final
	{
		assert(p != embvm::gpio::pull::pulldown && "Aardvark adapter does not support pulldowns\n");

		master_.pullup(TPinID, p == embvm::gpio::pull::pullup ? true : false);

		return p;
	}

  private:
	/// The aardvarkAdapter instance associated with this GPIO input.
	aardvarkAdapter& master_;
};

/** Aardvark GPIO Output driver
 *
 * Create a GPIO output driver using the Aardvark adapter.
 *
 * This driver requires an aardvarkAdapter to work. The aardvark adapter must be
 * configured with aardvarkMode::GpioI2C, aardvarkMode::SpiGpio, or aardvarkMode::GpioOnly.
 *
 * @code
 * embdrv::aardvarkAdapter aardvark{embdrv::aardvarkMode::GpioI2C};
 * embdrv::aardvarkGPIOOutput<5> gpio5{aardvark};
 * @endcode
 *
 * @tparam TPinID The aardvark pin ID to use as a GPIO output. Must be less than AARDVARK_IO_COUNT.
 * @ingroup AardvarkDrivers
 */
template<uint8_t TPinID>
class aardvarkGPIOOutput : public embvm::gpio::output
{
  public:
	/** Construct a generic GPIO output
	 *
	 * @param master The aardvarkAdapter instance associated with this GPIO pin.
	 */
	explicit aardvarkGPIOOutput(aardvarkAdapter& master) noexcept
		: embvm::gpio::output("Aardvark GPIO"), master_(master)
	{
		assert(TPinID < AARDVARK_IO_COUNT && "Invalid ID");
	}

	/** Construct a named GPIO output
	 *
	 * @param master The aardvarkAdapter instance associated with this GPIO pin.
	 * @param name The name of the GPIO pin
	 */
	explicit aardvarkGPIOOutput(aardvarkAdapter& master, const char* name) noexcept
		: embvm::gpio::output(name), master_(master)
	{
		assert(TPinID < AARDVARK_IO_COUNT && "Invalid ID");
	}

	/// Default destructor
	~aardvarkGPIOOutput() noexcept final = default;

	void set(bool v) noexcept final
	{
		if(embvm::DriverBase::started())
		{
			master_.lock();
			int r = aa_gpio_set(master_.handle(), v ? aardvarkIO[TPinID] : 0);
			master_.unlock();
			assert(r == AA_OK && "Error setting GPIO");
		}
	}

  private:
	void start_() noexcept final
	{
		master_.start();

		master_.lock();
		int r = aa_gpio_direction(master_.handle(), aardvarkIO[TPinID]);
		master_.unlock();
		assert(r == AA_OK && "Error setting GPIO Direction");
	}

	void stop_() noexcept final
	{
		master_.stop();
	}

  private:
	/// The aardvarkAdapter instance associated with this GPIO output.
	aardvarkAdapter& master_;
};

} // namespace embdrv

#endif // AARDVARK_GPIO_HPP_
