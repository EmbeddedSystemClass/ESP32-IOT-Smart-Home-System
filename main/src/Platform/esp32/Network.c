/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#include "Network.h"


int esp32_read(Network* network, unsigned char* buffer, int len, int timeout_ms)
{

	return 0;
}


int esp32_write(Network* network, unsigned char* buffer, int len, int timeout_ms)
{

	return 0;
}


void NetworkInit(Network* network)
{

}


int NetworkConnect(Network* network, char* addr, int port)
{

	return 0;
}


void NetworkDisconnect(Network* network)
{

}


int NetworkConnected(Network* network)
{
	return 0;
}