#pragma once

#include "AbstractMetadata.h"

template <typename T>
class DynamicMetadata : public AbstractMetadata {
protected:
	T value;
public:
	DynamicMetadata(T value) : value(value) {}
	virtual ~DynamicMetadata() {}
	T& getValue() { return value; }
};