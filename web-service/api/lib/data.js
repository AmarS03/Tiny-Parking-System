// In-memory store (reset on restart)
export const store = {
    boardStatus: {
        status: "initialized",
    },
    logs: [],
    spots: [],
    openingTime: "08:00",
    closingTime: "20:00",
}

export function updateStore(status) {
    store.boardStatus = status.boardStatus;
    store.logs = [];
    store.spots = initSpots(status.parkingSpots);
    store.openingTime = status.openingTime;
    store.closingTime = status.closingTime;
}

export function initSpots(parkingSpots) {
    return Array.from({ length: parkingSpots }, (_, i) => ({
        id: i + 1,
        occupiedBy: "",
    }));
}

export function getBoardStatus() {
    return store.boardStatus;
}

export function getLogs() {
    return store.logs;
}

export function getSpots() {
    return store.spots;
}

export function getOpeningHours() {
    return {
        openingTime: store.openingTime,
        closingTime: store.closingTime,
    };
}
