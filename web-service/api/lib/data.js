// Default number of parking spots
const defaultParkingSpots = 10;

// In-memory store (resets on restart)
const store = {
    boardStatus: [
        { name: "ESP main module", status: "No connection", espStatus: "Awaiting system connection" },
        { name: "Ultrasonic sensor", status: "No connection", espStatus: "Awaiting system connection" },
        { name: "Weight sensor", status: "No connection", espStatus: "Awaiting system connection" },
        { name: "Motor sensor", status: "No connection", espStatus: "Awaiting system connection" },
        { name: "Wifi sensor", status: "No connection", espStatus: "Awaiting system connection" },
        { name: "OLED display", status: "No connection", espStatus: "Awaiting system connection" },
    ],
    logs: [],
    spots: Array.from({ length: defaultParkingSpots }, (_, i) => ({
        id: i + 1,
        isOccupied: false,
        occupiedBy: '',
        occupiedAt: null,
    })),
    allowedPlates: [],
    openingTime: "08:00",
    closingTime: "22:00",
    initializedAt: new Date().toISOString(),
    lastUpdatedAt: new Date().toISOString(),
}

function getStore() {
    return store;
}

function addNewLog(type, message, imageUrl = null) {
    const logEntry = {
        type,
        message,
        imageUrl,
        timestamp: new Date().toISOString()
    };

    store.logs.push(logEntry);
    store.lastUpdatedAt = new Date().toISOString();
}

function setBoardStatus(updatedStatus) {
    store.boardStatus = updatedStatus;
    store.lastUpdatedAt = new Date().toISOString();
}

function setAllowedLicensePlates(plates) {
    store.allowedPlates = plates;
    store.lastUpdatedAt = new Date().toISOString();
}

function noOccupiedSpots() {
    return store.spots.every(spot => !spot.isOccupied);
}

function parkVehicle(licensePlate) {
    const freeSpot = store.spots.find(spot => !spot.isOccupied);

    if (freeSpot) {
        freeSpot.isOccupied = true;
        freeSpot.occupiedBy = licensePlate;
        freeSpot.occupiedAt = new Date().toISOString();
        store.lastUpdatedAt = new Date().toISOString();

        return true;
    } else {
        return false;
    }
}

function removeParkedVehicle(licensePlate) {
    const spot = store.spots.find(spot => spot.occupiedBy === licensePlate);

    if (spot) {
        spot.isOccupied = false;
        spot.occupiedBy = '';
        spot.occupiedAt = null;
        store.lastUpdatedAt = new Date().toISOString();

        return true;
    } else {
        return false;
    }
}

function isLicensePlateAllowed(licensePlate) {
    return store.allowedPlates.includes(licensePlate);
}

module.exports = {
    defaultParkingSpots,
    getStore,
    addNewLog,
    setBoardStatus,
    setAllowedLicensePlates,
    noOccupiedSpots,
    parkVehicle,
    removeParkedVehicle,
    isLicensePlateAllowed,
};