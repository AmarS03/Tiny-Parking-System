"use client";

import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { CarFront, TextSearchIcon } from 'lucide-react';

export interface ParkingSpot {
    id: number;
    occupied: boolean;
    licensePlate?: string;
}

export function ParkingLotSection() {
    const spots : ParkingSpot[] = [
        { id: 1, occupied: true, licensePlate: 'ABC-1234' },
        { id: 2, occupied: false },
        { id: 3, occupied: true, licensePlate: 'XYZ-5678' },
        { id: 4, occupied: false },
        { id: 5, occupied: false },
        { id: 6, occupied: true, licensePlate: 'LMN-9101' },
        { id: 7, occupied: false },
        { id: 8, occupied: true, licensePlate: 'DEF-2345' },
        { id: 9, occupied: false },
        { id: 10, occupied: false },
    ];

    const toggleSpot = (id: number) => {
        const spotIndex = spots.findIndex(s => s.id === id);
        if (spotIndex !== -1) {
            spots[spotIndex].occupied = !spots[spotIndex].occupied;
        }
    };

    const occupiedCount = spots.filter(s => s.occupied).length;

    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow overflow-hidden flex flex-col">
            <CardHeader className="pb-2">
                <div className="flex justify-between items-center">
                    <CardTitle className="flex items-center gap-2">
                        <CarFront className="h-6 w-6 text-indigo-500" />
                        Live Parking Overview
                    </CardTitle>
                    <Badge variant="outline" className="text-sm">
                        Occupancy: {occupiedCount}/{spots.length}
                    </Badge>
                </div>
            </CardHeader>
            <CardContent className="flex-1 flex flex-col justify-center p-6 bg-slate-50/50 dark:bg-slate-900/20 perspective-1000">
                <div className="grid grid-cols-3 sm:grid-cols-4 gap-4 w-full h-full max-h-[500px]" style={{ transform: "rotateX(20deg) scale(0.95)" }}>
                    {spots.map((spot) => (
                        <div
                            key={spot.id}
                            onClick={() => toggleSpot(spot.id)}
                            className={`
                        relative group cursor-pointer transition-all duration-300 ease-out
                        rounded-lg border-2 shadow-lg flex flex-col items-center justify-center p-2 min-h-[100px]
                        hover:scale-105 hover:shadow-xl hover:z-10
                        ${spot.occupied
                                    ? "bg-red-50 border-red-200 dark:bg-red-950/30 dark:border-red-900"
                                    : "bg-emerald-50 border-emerald-200 dark:bg-emerald-950/30 dark:border-emerald-900"}
                    `}
                        >
                            {/* Spot Number */}
                            <span className="absolute top-2 left-2 text-xs font-bold opacity-50">#{spot.id}</span>

                            {/* Indicator Circle */}
                            <div className={`
                        w-12 h-12 rounded-full shadow-inner mb-2 flex items-center justify-center
                        transition-colors duration-500
                        ${spot.occupied ? "bg-red-500 shadow-red-600/50" : "bg-emerald-400 shadow-emerald-500/50"}
                    `}>
                                {spot.occupied && <CarFront className="text-white w-6 h-6" />}
                            </div>

                            {/* Status Text / License Plate */}
                            {spot.occupied ? (
                                <div className="absolute inset-0 flex items-center justify-center pointer-events-none">
                                    <span className="
                                -rotate-12 bg-white/90 dark:bg-black/80 px-2 py-1 rounded border shadow-sm
                                font-mono text-xs sm:text-sm font-bold tracking-wider text-slate-800 dark:text-slate-200
                                transform translate-y-4
                            ">
                                        {spot.licensePlate}
                                    </span>
                                </div>
                            ) : (
                                <span className="text-emerald-600 dark:text-emerald-400 font-semibold text-sm">Free</span>
                            )}
                        </div>
                    ))}
                </div>
                <div className="text-center mt-4 text-xs text-muted-foreground italic">
                    * Click spot to toggle occupancy simulation
                </div>
            </CardContent>
        </Card>
    );
}
