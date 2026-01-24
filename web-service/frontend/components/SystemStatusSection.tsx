"use client";

import { ModuleStatusCard } from './SystemStatus/ModuleStatusCard';
import { Card, CardHeader, CardTitle, CardContent, CardDescription } from '@/components/ui/card';
import { Activity } from 'lucide-react';

export type sensorStatus = {
    name: string;
    status: "No connection" | "Active" | "Problem";
    message?: string;
};

export function SystemStatusSection(
    {
        sensorData,
        lastUpdated,
        timeDifference
    } : {
        sensorData: sensorStatus[],
        lastUpdated: string,
        timeDifference: string
    }
) {
    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow">
            <CardHeader>
                <CardTitle className="flex items-center text-xl gap-2">
                    <Activity className="h-6 w-6 text-green-500" />
                    System status
                </CardTitle>
                <CardDescription className='text-muted-foreground'>
                    <span className='text-black font-medium'>Last updated:</span> {lastUpdated}
                    <br />
                    <span className='text-xs'>({timeDifference})</span>
                </CardDescription>
            </CardHeader>
            <CardContent>
                <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4 2xl:grid-cols-5 gap-4">
                    {
                        sensorData.map((sensor, index) => (
                            <ModuleStatusCard key={index} sensor={sensor} />
                        ))
                    }
                </div>
            </CardContent>
        </Card>
    );
}
