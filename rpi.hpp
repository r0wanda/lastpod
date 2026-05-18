#pragma once
#include <chrono>
#include <string>
#include <thread>
#include <sstream>
#include <fstream>

class Leds {
public:
	Leds() {
		og0 = ledInit("/sys/class/leds/led0/trigger");
		og1 = ledInit("/sys/class/leds/led1/trigger");
		blink(4, 100, 100);
	}
	void blink(unsigned int n, unsigned int del, unsigned int deloff) {
		for (int i = 0; i < n; i++) {
			onBoth();
			mleep(del);
			offBoth();
			mleep(deloff);
		}
	}
	void blinkRed(unsigned int n, unsigned int del, unsigned int deloff) {
		for (int i = 0; i < n; i++) {
			onRed();
			mleep(del);
			offRed();
			mleep(deloff);
		}
	}
	void blinkGreen(unsigned int n, unsigned int del, unsigned int deloff) {
		for (int i = 0; i < n; i++) {
			onGreen();
			mleep(del);
			offGreen();
			mleep(deloff);
		}
	}
	void close() {
		blinkRed(1, 750, 500);
		ledClose("/sys/class/leds/led0/trigger", og0);
		ledClose("/sys/class/leds/led1/trigger", og1);
	}
	void onBoth() {
		onRed();
		onGreen();
	}
	void offBoth() {
		offRed();
		offGreen();
	}
	void onGreen() {
		on("/sys/class/leds/led0/brightness");
	}
	void onRed() {
		on("/sys/class/leds/led1/brightness");
	}
	void offGreen() {
		off("/sys/class/leds/led0/brightness");
	}
	void offRed() {
		off("/sys/class/leds/led1/brightness");
	}
private:
	std::string og0;
	std::string og1;
	std::string ledInit(std::string path) {
		std::fstream fstrm(path);
		std::ostringstream sstrm;
		sstrm << fstrm.rdbuf();
		std::string og = sstrm.str();

		fstrm << "none";
		fstrm.close();

		return og;
	}
	void ledClose(std::string path, std::string og) {
		std::ofstream fstrm(path);
		fstrm << og;
		fstrm.close();
	}
	void on(std::string path) {
		std::ofstream fstrm(path);
		fstrm << "1";
		fstrm.close();
	}
	void off(std::string path) {
		std::ofstream fstrm(path);
		fstrm << "0";
		fstrm.close();
	}
	void mleep(unsigned int ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
};
