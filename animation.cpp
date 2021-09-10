#include "animation.h"

Animation::Animation(string name) {
	this->name = name;
}

int Animation::getNextFrameNumber(int frameNumber) {
	if ((frameNumber + 1) < frames.size()) {
		return frameNumber + 1;
	}
	else {
		return 0;
	}
}

Frame* Animation::getNextFrame(Frame* frame) {
	return getFrame(getNextFrameNumber(frame->frameNumber));
}

int Animation::getEndFrameNumber() {
	return frames.size() - 1;
}

Frame* Animation::getFrame(int frameNumber) {
	if (frames.size() == 0) {
		return NULL;
	}

	list<Frame>::iterator i = frames.begin(); // point iterator to first frame
	int counter = 0;
	for (counter = 0; counter < frameNumber && counter < frames.size() - 1; counter++) {
		i++; // make iterator point to next frame
	}

	Frame *frame = &(*i); // make frame point to frame inside the list our iterator is pointing
						//*i = gets frame at i`s position in the list
						//& = get the memory addr of *i
	return frame;
}

void Animation::loadAnimation(ifstream& file, list<DataGroupType>& groupTypes) {
	getline(file, name);
	string buffer;
	getline(file, buffer);
	stringstream ss;
	buffer = Globals::clipOffDataHeader(buffer);
	ss << buffer;
	int numberOfFrames;
	ss >> numberOfFrames;
	for (int i = 0; i < numberOfFrames; i++) {
		Frame newFrame;
		newFrame.loadFrame(file, groupTypes);
		frames.push_back(newFrame);
	}
}