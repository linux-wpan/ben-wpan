/*
 * lib/atrf.c - ATRF access functions library
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>

#include "driver.h"
#include "atrf.h"


extern struct atrf_driver atusb_driver;
extern struct atrf_driver atusd_driver;


struct atrf_dsc {
	struct atrf_driver *driver;
	void *handle;
};


void *atrf_usb_handle(struct atrf_dsc *dsc)
{
#ifdef HAVE_USB
	return dsc->handle;
#else
	return NULL;
#endif
}


int atrf_error(struct atrf_dsc *dsc)
{
	return dsc->driver->error ? dsc->driver->error(dsc->handle) : 0;
}


int atrf_clear_error(struct atrf_dsc *dsc)
{
	return dsc->driver->clear_error ?
	    dsc->driver->clear_error(dsc->handle) : 0;
}


struct atrf_dsc *atrf_open(void)
{
	struct atrf_dsc *dsc;
	struct atrf_driver *driver;
	void *handle;

#ifdef HAVE_USB
	driver = &atusb_driver;
#elif HAVE_USD
	driver = &atusd_driver;
#else
#error Need either HAVE_USB or HAVE_USD
#endif
	handle = driver->open();
	if (!handle)
		return NULL;
	dsc = malloc(sizeof(*dsc));
	if (!dsc) {
		perror("malloc");
		exit(1);
	}
	dsc->driver = driver;
	dsc->handle = handle;
	return dsc;
}


void atrf_close(struct atrf_dsc *dsc)
{
	if (dsc->driver->close)
		dsc->driver->close(dsc->handle);
	free(dsc);
}


void atrf_reset(struct atrf_dsc *dsc)
{
	if (dsc->driver->reset)
		dsc->driver->reset(dsc->handle);
}


void atrf_reset_rf(struct atrf_dsc *dsc)
{
	dsc->driver->reset_rf(dsc->handle);
}


int atrf_test_mode(struct atrf_dsc *dsc)
{
	if (!dsc->driver->test_mode)
		return 0;
	dsc->driver->test_mode(dsc->handle);
	return 1;
}


void atrf_reg_write(struct atrf_dsc *dsc, uint8_t reg, uint8_t value)
{
	dsc->driver->reg_write(dsc->handle, reg, value);
}


uint8_t atrf_reg_read(struct atrf_dsc *dsc, uint8_t reg)
{
	return dsc->driver->reg_read(dsc->handle, reg);
}


void atrf_buf_write(struct atrf_dsc *dsc, const void *buf, int size)
{
	dsc->driver->buf_write(dsc->handle, buf, size);
}


int atrf_buf_read(struct atrf_dsc *dsc, void *buf, int size)
{
	return dsc->driver->buf_read(dsc->handle, buf, size);
}


int atrf_interrupt(struct atrf_dsc *dsc)
{
	return
	    dsc->driver->interrupt ? dsc->driver->interrupt(dsc->handle) : 1;
}
