// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef AARDVARK_I2C_DRIVER_HPP_
#define AARDVARK_I2C_DRIVER_HPP_

#include "base.hpp"
#include <active_object/active_object.hpp>
#include <cstdint>
#include <driver/i2c.hpp>

namespace embdrv
{
/** Create an Aardvark I2C Master Driver
 *
 * This driver requires an aardvarkAdapter to work. The aardvark adapter must be
 * configured with aardvarkMode::GpioI2C or aardvarkMode::SpiI2C.
 *
 * This is an active object: it has its own thread of control.
 *
 * @code
 * embdrv::aardvarkAdapter aardvark{embdrv::aardvarkMode::GpioI2C};
 * embdrv::aardvarkI2CMaster i2c0{aardvark};
 * @endcode
 *
 * @ingroup AardvarkDrivers
 */
class aardvarkI2CMaster final
	: public embvm::i2c::master,
	  public embutil::activeObject<aardvarkI2CMaster,
								   std::pair<embvm::i2c::op_t, const embvm::i2c::master::cb_t>>
{
	/// The storage type that the active object stores.
	using storagePair_t = std::pair<embvm::i2c::op_t, const embvm::i2c::master::cb_t>;

  public:
	/** Construct an Aardvark I2C master
	 *
	 * @param base_driver The aardvarkAdapter instance associated with this driver.
	 */
	explicit aardvarkI2CMaster(aardvarkAdapter& base_driver) noexcept : base_driver_(base_driver) {}

	/// Default destructor
	~aardvarkI2CMaster() noexcept;

	/// Active object process function
	void process_(const storagePair_t& op) noexcept;

  private:
	void configure_(embvm::i2c::pullups pullup) noexcept final;
	embvm::i2c::status transfer_(const embvm::i2c::op_t& op,
								 const embvm::i2c::master::cb_t& cb) noexcept final;
	embvm::i2c::baud baudrate_(embvm::i2c::baud baud) noexcept final;
	embvm::i2c::pullups setPullups_(embvm::i2c::pullups pullups) noexcept final;
	void start_() noexcept final;
	void stop_() noexcept final;

  private:
	/// The aardvarkAdapter instance associated with this driver.
	aardvarkAdapter& base_driver_;
};

} // namespace embdrv

#endif // AARDVARK_I2C_DRIVER_HPP_
