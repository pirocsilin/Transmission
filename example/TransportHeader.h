#ifndef _RDMP_TRANSPORT_HEADER_H_
#define _RDMP_TRANSPORT_HEADER_H_

#include <stdint.h>

#pragma pack (push, 1)

namespace Rdmp
{
/// Транспортный заголовок
struct TransportHeader
{
	/// 4 байтa - размер следующего за заголовком protobuf-сообщения
	uint32_t protobufLength;
    /// 8 байт - размер следующего за protobuf-сообщением бинарного 
    /// вложения
    uint64_t binaryAttachmentLength{};
	/// Версия протокола (RdmpVersion)
	uint16_t version;

	/// Существующие версии протокола
	enum RdmpVersion
	{
		/// Версия 1.0 15.07.2013
		/// Используется Google protobuf версии 2.5.0
		V_1_0 = 0x0100
	};
};
}

#pragma pack (pop)

#endif // _UAV_TRANSPORT_HEADER_H_


