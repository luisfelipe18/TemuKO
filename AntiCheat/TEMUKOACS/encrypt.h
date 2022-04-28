#pragma once
#include "stdafx.h"

void encrypt(char* str) {
	while (*str != '\0') {
		*str ^= 18;
		str++;
	}
}

void decrypt(char* str) {
	while (*str != '\0') {
		*str ^= 18;
		str++;
	}
}