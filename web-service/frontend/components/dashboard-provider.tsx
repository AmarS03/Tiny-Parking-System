"use client";

import React, { createContext, useContext, ReactNode, useEffect } from 'react';
import { useLocalStorage } from '@/hooks/useLocalStorage';
import { Sensor } from '@/components/dashboard/SensorStatusCard';
import { ParkingSpot } from '@/components/dashboard/ParkingLotCard';
import { ActivityLog } from '@/components/dashboard/ActivityLogCard';

// Define default constants here to avoid circular auth dependencies or repetition
const DEFAULT_SENSORS: Sensor[] = [
    { id: '1', name: 'Entrance Weight Sensor', type: 'weight', status: 'active' },
    { id: '2', name: 'Exit Ultrasonic Sensor', type: 'ultrasonic', status: 'active' },
    { id: '3', name: 'Camera Vision A', type: 'camera', status: 'active' },
    { id: '4', name: 'Perimeter Laser', type: 'generic', status: 'inactive' },
];

const INITIAL_SPOTS: ParkingSpot[] = Array.from({ length: 12 }, (_, i) => ({
    id: i + 1,
    occupied: false
}));

interface DashboardContextType {
    sensors: Sensor[];
    setSensors: (value: Sensor[] | ((val: Sensor[]) => Sensor[])) => void;

    spots: ParkingSpot[];
    toggleSpot: (id: number) => void;

    isBarrierOpen: boolean;
    toggleBarrier: (isOpen: boolean) => void;

    logs: ActivityLog[];
    addLog: (message: string, type: ActivityLog['type']) => void;
}

const DashboardContext = createContext<DashboardContextType | undefined>(undefined);

export function DashboardProvider({ children }: { children: ReactNode }) {
    // 1. Sensors State
    const [sensors, setSensors] = useLocalStorage<Sensor[]>('parking_sensors', DEFAULT_SENSORS);

    // 2. Parking Spots State
    const [spots, setSpots] = useLocalStorage<ParkingSpot[]>('parking_spots', INITIAL_SPOTS);

    // 3. Barrier State
    const [isBarrierOpen, setIsBarrierOpen] = useLocalStorage<boolean>('barrier_state', false);

    // 4. Activity Logs State
    const [logs, setLogs] = useLocalStorage<ActivityLog[]>('activity_logs', []);


    // --- Actions ---

    const addLog = (message: string, type: ActivityLog['type']) => {
        const newLog: ActivityLog = {
            id: crypto.randomUUID(),
            timestamp: new Date().toISOString(),
            message,
            type
        };
        setLogs(prev => [...prev.slice(-49), newLog]); // Keep last 50
    };


    const toggleBarrier = (isOpen: boolean) => {
        setIsBarrierOpen(isOpen);
        addLog(
            isOpen ? "Main Barrier Gate OPENED" : "Main Barrier Gate CLOSED",
            isOpen ? "warning" : "success"
        );
    };

    // Sensor Simulation Logic (Moved from Card to Context to persist logic globally)
    useEffect(() => {
        const interval = setInterval(() => {
            setSensors((prev) => {
                if (Math.random() > 0.3) return prev; // Lower frequency
                const index = Math.floor(Math.random() * prev.length);
                const newSensors = [...prev];
                // Only toggle status occasionally
                const newStatus = newSensors[index].status === 'active' ? 'inactive' : 'active';
                newSensors[index] = { ...newSensors[index], status: newStatus };
                return newSensors;
            });
        }, 5000);
        return () => clearInterval(interval);
    }, [setSensors]);


    const toggleSpot = (id: number) => {
        let action: { occupied: boolean, licensePlate?: string } | null = null;

        setSpots(prev => prev.map(spot => {
            if (spot.id !== id) return spot;

            const newOccupied = !spot.occupied;
            const licensePlate = newOccupied
                ? `${String.fromCharCode(65 + Math.floor(Math.random() * 26))}${String.fromCharCode(65 + Math.floor(Math.random() * 26))}${100 + Math.floor(Math.random() * 899)}${String.fromCharCode(65 + Math.floor(Math.random() * 26))}${String.fromCharCode(65 + Math.floor(Math.random() * 26))}`
                : undefined;

            action = { occupied: newOccupied, licensePlate };
            return { ...spot, occupied: newOccupied, licensePlate };
        }));

        if (action) {
            const { occupied, licensePlate } = action;
            addLog(
                occupied ? `Spot #${id} OCCUPIED by ${licensePlate}` : `Spot #${id} VACATED`,
                occupied ? 'info' : 'success'
            );

        }
    };

    return (
        <DashboardContext.Provider value={{
            sensors, setSensors,
            spots, toggleSpot,
            isBarrierOpen, toggleBarrier,
            logs, addLog
        }}>
            {children}
        </DashboardContext.Provider>
    );
}

export function useDashboard() {
    const context = useContext(DashboardContext);
    if (context === undefined) {
        throw new Error('useDashboard must be used within a DashboardProvider');
    }
    return context;
}
