/*
  WiFiClient.cpp - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino LLC.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

extern "C" {
#include "string.h"
#include "utility/debug.h"
#include "utility/wifi_spi.h"
#include "utility/wl_definitions.h"
}

#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "logging.h"
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

WiFiClient::WiFiClient(int sock) : psock(sock) {
  if (psock) {
    int flags = fcntl(psock, F_GETFL, 0);
    fcntl(psock, F_SETFL, flags | O_NONBLOCK);
    errorCode = 0;
  }
}

int WiFiClient::connect(const char *host, uint16_t port) {
  notImplemented("WiFiClient::connect");
  return 0;
}

int WiFiClient::connect(IPAddress ip, uint16_t port) {
  notImplemented("WiFiClient::connect");
  return 0;
}

size_t WiFiClient::write(uint8_t b) { return write(&b, 1); }

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
  if (!psock) {
    setWriteError();
    return 0;
  }
  if (size == 0) {
    setWriteError();
    return 0;
  }

  ::write(psock, buf, size);

#if 0
  if (!ServerDrv::sendData(_sock, buf, size))
  {
	  setWriteError();
      return 0;
  }
  if (!ServerDrv::checkDataSent(_sock))
  {
	  setWriteError();
      return 0;
  }
#endif

  return size;
}

int WiFiClient::available() {
  if (psock) {
    if (ready == -1) { // No char already read, ask the OS
      uint8_t b;
      int numread = ::read(psock, &b, 1);
      if (numread < 1) {
        errorCode = errno;
        // EAGAIN means timeout
        if (errorCode == EAGAIN)
          errorCode = 0;

        ready = -1;
      } else {
        ready = b;
      }
    }

    int len = (ready == -1) ? 0 : 1;

    // log(SysWifi, LogVerbose, "available %d", len);
    return len;
  }

  return 0;
}

int WiFiClient::read() {
  char b;
  if (!available())
    return -1;

  b = ready;
  ready = -1; // consumed
  return (uint8_t)b;
}

int WiFiClient::read(uint8_t *buf, size_t size) {
  assert(psock);
  // FIXME - this is super inefficient, instead do block reads from the file
  // descriptor
  auto r = ::read(psock, buf, size);
  errorCode = errno;

  return r;
}

int WiFiClient::peek() {
  uint8_t b;
  if (!available())
    return -1;

  b = ready;
  return b;
}

void WiFiClient::flush() { assert(psock); }

void WiFiClient::stop() {
  if (!psock)
    return;

  ::close(psock);
  psock = 0;
}

uint8_t WiFiClient::connected() {

  if (!psock) {
    return 0;
  } else {
    return errorCode == 0;
  }
}

uint8_t WiFiClient::status() {
  if (!psock) {
    return CLOSED;
  } else {
    return ESTABLISHED;
  }
}

WiFiClient::operator bool() { return connected(); }
