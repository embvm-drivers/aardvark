#include <aardvark/spi.hpp>
#include "vendor/aardvark.h"
#include <vector>

using namespace embdrv;

constexpr unsigned INPUT_BAUDRATE_TO_AARDVARK_CONV_FACTOR = 1000;

aardvarkSPIMaster::~aardvarkSPIMaster() noexcept = default;

void aardvarkSPIMaster::start_() noexcept
{
	base_driver_.start();
}

void aardvarkSPIMaster::stop_() noexcept
{
	base_driver_.stop();
}

uint32_t aardvarkSPIMaster::baudrate_(uint32_t baud) noexcept
{
	base_driver_.lock();
	auto set_baud = aa_spi_bitrate(base_driver_.handle(),
								   static_cast<int>(baud / INPUT_BAUDRATE_TO_AARDVARK_CONV_FACTOR));
	base_driver_.unlock();

	assert(set_baud == static_cast<int>(baud / INPUT_BAUDRATE_TO_AARDVARK_CONV_FACTOR));
	// TODO: tolerance checks

	return baud;
}

void aardvarkSPIMaster::configure_() noexcept
{
	if(started())
	{
		auto bitorder =
			order_ == embvm::spi::order::msbFirst ? AA_SPI_BITORDER_MSB : AA_SPI_BITORDER_LSB;
		base_driver_.lock();
		aa_spi_configure(base_driver_.handle(),
						 static_cast<AardvarkSpiPolarity>(static_cast<int>(mode_) >> 1),
						 static_cast<AardvarkSpiPhase>(static_cast<int>(mode_) & 1), bitorder);
		base_driver_.unlock();
	}
}

embvm::comm::status aardvarkSPIMaster::transfer_(const embvm::spi::op_t& op,
												 const embvm::spi::master::cb_t& cb) noexcept
{
	enqueue({op, cb});

	return embvm::comm::status::enqueued;
}

void aardvarkSPIMaster::process_(const storagePair_t& pair) noexcept
{
	const auto& [op, cb] = pair;

	std::vector<uint8_t> zeroes(op.length, 0);

	auto* rx_buffer = (op.rx_buffer != nullptr) ? op.rx_buffer : zeroes.data();
	auto* tx_buffer = (op.tx_buffer != nullptr) ? op.tx_buffer : zeroes.data();

	base_driver_.lock();
	int r = aa_spi_write(base_driver_.handle(), static_cast<uint16_t>(op.length), tx_buffer,
						 static_cast<uint16_t>(op.length), rx_buffer);
	base_driver_.unlock();

	embvm::comm::status status;

	switch(r)
	{
		case AA_OK:
			status = embvm::comm::status::ok;
			break;
		case AA_SPI_NOT_AVAILABLE:
			status = embvm::comm::status::busy;
			break;
		case AA_SPI_WRITE_ERROR:
		case AA_SPI_SLAVE_READ_ERROR:
		case AA_SPI_SLAVE_TIMEOUT:
		case AA_SPI_DROPPED_EXCESS_BYTES:
			status = embvm::comm::status::error;
			break;
		case AA_SPI_NOT_ENABLED:
		default:
			status = embvm::comm::status::unknown;
	}

	callback(op, status, cb);
}

void aardvarkSPIMaster::setMode_(embvm::spi::mode mode) noexcept
{
	assert(((mode == embvm::spi::mode::mode0) || (mode == embvm::spi::mode::mode3)) &&
		   "Aardvark only supports SPI mode 3 and 0");
	mode_ = mode;
	configure_();
}

void aardvarkSPIMaster::setOrder_(embvm::spi::order order) noexcept
{
	order_ = order;
	configure_();
}
