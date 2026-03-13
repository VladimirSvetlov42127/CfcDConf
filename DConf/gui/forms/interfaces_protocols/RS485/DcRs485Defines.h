#pragma once

#include <stdint.h>

namespace Rs485 {
	namespace Indexs {
		namespace Byte {
			enum {
				Protocol = 0,
				FormatParity = 1,
				FormatLenght = 2,
				FormatStopBits = 3,
				SlaveCount = 4,
				Retries = 5
			};
		}

		namespace Word {
			enum {
				Address = 0,
				Timeout = 1,
				Interval = 2,
				Options = 3,
				InputBuffer = 4,
                OutputBuffer = 5,
                TcpIpPort = 6
			};
		}

		namespace DWord {
			enum {
				Speed = 0,
				Options = 1
			};
		}
	}

	namespace Parity {
		enum {
			No = 0, 
			Odd,
			Even
		};
	}

	namespace Format {
		enum {
			F8Bit = 8,
			F9Bit = 9
		};
	}

	namespace Text {
		const QString TransparentMode = "Прозрачный режим интерфейса";
		const QString PollingInterval = "Интервал опроса (мс)";
		const QString TimeoutResponse = "Таймаут ответа (мс)";
		const QString SlavesCount = "Количество ведомых";
		const QString SlavesRetries = "Количество повторов запроса к ведомым";
		const QString CharacterFormat = "Формат символа";
		const QString FrameInterval = "Межкадровый интервал";		
	}
}
