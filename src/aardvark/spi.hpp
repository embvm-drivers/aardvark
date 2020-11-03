// Copyright 2020 Embedded Artistry LLC
// SPDX-License-Identifier: MIT

#ifndef AARDVARK_SPI_DRIVER_HPP_
#define AARDVARK_SPI_DRIVER_HPP_

#include "base.hpp"
#include <active_object/active_object.hpp>
#include <cstdint>
#include <driver/spi.hpp>

namespace embdrv
{
/** Create an Aardvark SPI Master Driver
 *
 * This driver requires an aardvarkAdapter to work. The aardvark adapter must be
 * configured with aardvarkMode::SpiGpio or aardvarkMode::SpiI2C.
 *
 * This is an active object: it has its own thread of control.
 *
 * @code
 * embdrv::aardvarkAdapter aardvark{embdrv::aardvarkMode::SpiI2C};
 * embdrv::aardvarkSPIMaster spi0{aardvark, "spi0"};
 * @endcode
 *
 * @ingroup AardvarkDrivers
 */
class aardvarkSPIMaster final
	: public embvm::spi::master,
	  public embutil::activeObject<aardvarkSPIMaster,
								   std::pair<embvm::spi::op_t, const embvm::spi::master::cb_t>>
{
	/// The storage type that the active object stores.
	using storagePair_t = std::pair<embvm::spi::op_t, const embvm::spi::master::cb_t>;

  public:
	/** Construct an Aardvark SPI master
	 *
	 * @param base_driver The aardvarkAdapter instance associated with this driver.
	 */
	explicit aardvarkSPIMaster(aardvarkAdapter& base_driver) noexcept : base_driver_(base_driver) {}

	/// Default destructor.
	~aardvarkSPIMaster() noexcept;

	/// Active object process function
	void process_(const storagePair_t& op) noexcept;

  private:
	void start_() noexcept final;
	void stop_() noexcept final;
	void configure_() noexcept final;
	uint32_t baudrate_(uint32_t baud) noexcept final;
	embvm::comm::status transfer_(const embvm::spi::op_t& op,
								  const embvm::spi::master::cb_t& cb) noexcept final;
	void setMode_(embvm::spi::mode mode) noexcept final;
	void setOrder_(embvm::spi::order order) noexcept final;

  private:
	/// The aardvarkAdapter instance associated with this driver.
	aardvarkAdapter& base_driver_;
};

} // namespace embdrv

#endif // AARDVARK_SPI_DRIVER_HPP_
