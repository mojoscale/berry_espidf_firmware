#pragma once

enum class EventType {
    Boot,
    WifiConnecting,
    WifiConnected,
    WifiDisconnected,
    WifiBadCredentials,
    WifiProvisioningStarted,
    WifiProvisioningCompleted
};

struct Event {
    EventType type;
};