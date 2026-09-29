#pragma once
#include "LogEntry.h"

#include <vector>

// Can add other sinks like file sink for errors to file..
class ISink{
public:
    virtual ~ISink() = default;
    virtual void Write(const LogEntry& _entry) = 0;
    virtual void Clear() = 0;
};

class ConsoleSink : public ISink{
public:
    void Write(const LogEntry& _entry) override;
    void Clear() override;
};

class ImGuiSink : public ISink{
public:
    void Write(const LogEntry& _entry) override;
    void Clear() override;
    const std::vector<LogEntry>& GetEntries() const;

private:
    std::vector<LogEntry> mEntries;
};