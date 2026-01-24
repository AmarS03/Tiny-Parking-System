"use client";

import { useState } from 'react';
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { CarFront, ParkingMeterIcon } from 'lucide-react';
import { ResizableHandle, ResizablePanel, ResizablePanelGroup } from './ui/resizable';
import SplineComponent from './ParkingLot/SplineComponent';

export interface parkingSpot {
    id: number;
    isOccupied: boolean;
    occupiedBy?: string;
    occupiedAt?: string;
}

export function ParkingLotSection(
    {
        spotsData,
        allowedPlates
    } : {
        spotsData: parkingSpot[],
        allowedPlates: string[]
    }
) {
    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow overflow-hidden flex flex-col">
            <CardHeader className="pb-0">
                <CardTitle className="flex items-center text-xl gap-2">
                    <ParkingMeterIcon className="h-6 w-6 text-amber-500" />
                    Parking lot visualization
                </CardTitle>
            </CardHeader>
            <CardContent>
                <ResizablePanelGroup
                    direction="horizontal"
                    className="rounded-lg border"
                >
                    <ResizablePanel defaultSize={70}>
                        <div className="w-full h-full min-h-[500px]">
                            <SplineComponent />
                        </div>
                    </ResizablePanel>
                    <ResizableHandle />
                    <ResizablePanel defaultSize={30}>
                        <div className="grid grid-cols-1 xs:grid-cols-2 sm:grid-cols-3 gap-4 w-full h-full p-4" style={{ transform: "rotateX(20deg) scale(0.95)" }}>
                            {spotsData.map((spot) => (
                                <div
                                    key={spot.id}
                                    className={`
                                        relative group cursor-pointer transition-all duration-300 ease-out
                                        rounded-lg border-2 shadow-lg flex flex-col items-center justify-center p-2 min-h-[100px]
                                        hover:scale-105 hover:shadow-xl hover:z-10
                                        ${spot.isOccupied
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
                                        ${spot.isOccupied ? "bg-red-500 shadow-red-600/50" : "bg-emerald-400 shadow-emerald-500/50"}
                                    `}>
                                        {spot.isOccupied && <CarFront className="text-white w-6 h-6" />}
                                    </div>

                                    {/* Status Text / License Plate */}
                                    {spot.isOccupied ? (
                                        <div className="absolute inset-0 flex items-center justify-center pointer-events-none">
                                            <span className="
                                                -rotate-12 bg-white/90 dark:bg-black/80 px-2 py-1 rounded border shadow-sm
                                                font-mono text-xs sm:text-sm font-bold tracking-wider text-slate-800 dark:text-slate-200
                                                transform translate-y-4
                                            ">
                                                {spot.occupiedBy}
                                            </span>
                                        </div>
                                    ) : (
                                        <span className="text-emerald-600 dark:text-emerald-400 font-semibold text-sm">Free</span>
                                    )}
                                </div>
                            ))}
                        </div>
                    </ResizablePanel>
                </ResizablePanelGroup>
            </CardContent>
        </Card>
    );
}