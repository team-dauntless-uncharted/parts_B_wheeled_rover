#pragma once

class CdSSensor {
public:
	CdSSensor(int pin);
	int read();

private:
	int _pin;
};