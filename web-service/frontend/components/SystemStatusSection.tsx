"use client";

import { ModuleStatusCard } from './SystemStatus/ModuleStatusCard';
import { Card, CardHeader, CardTitle, CardContent, CardDescription } from '@/components/ui/card';
import { Activity } from 'lucide-react';
import { useEffect, useState } from 'react';

type SensorStatus = {
    name: string;
    status: "No connection" | "Active" | "Problem";
    message?: string;
};

function getTimeDifference(timestamp: string): string {
    const now = new Date();
    const updated = new Date(timestamp);
    const diffMs = now.getTime() - updated.getTime();
    const diffSeconds = Math.floor(diffMs / 1000);
    const diffMinutes = Math.floor(diffSeconds / 60);
    const diffHours = Math.floor(diffMinutes / 60);
    const diffDays = Math.floor(diffHours / 24);

    if (diffSeconds < 60) return 'just now';
    if (diffMinutes < 60) return `${diffMinutes} minute${diffMinutes !== 1 ? 's' : ''} ago`;
    if (diffHours < 24) return `over ${diffHours} hour${diffHours !== 1 ? 's' : ''} ago`;
    return `${diffDays} day${diffDays !== 1 ? 's' : ''} ago`;
};

export function SystemStatusSection() {
    const [sensors, setSensors] = useState<SensorStatus[]>([]);
    const [lastUpdated, setLastUpdated] = useState<string>('');
    const [timeDifference, setTimeDifference] = useState<string>('Awaiting data...');

    useEffect(() => {
        const fetchStatus = async () => {
            try {
                const response = await fetch('https://tinyparkingsystem-api.vercel.app/status');
                const data = await response.json();

                setSensors(data.boardStatus || []);
                setLastUpdated(new Date(data.lastUpdatedAt).toLocaleString('it-IT'));
                setTimeDifference(getTimeDifference(data.lastUpdatedAt));
            } catch (error) {
                console.error('Failed to fetch status:', error);
                
                setSensors([
                    { name: 'API', status: 'No connection', message: 'Unable to connect to API' },
                ]);
                setLastUpdated(new Date().toLocaleString('it-IT'));
                setTimeDifference('Unable to connect');
            }
        };

        fetchStatus();
        const interval = setInterval(fetchStatus, 10000); // Refreshes every 10 seconds
        return () => clearInterval(interval);
    }, []);

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
                        sensors.map((sensor, index) => (
                            <ModuleStatusCard key={index} sensor={sensor} />
                        ))
                    }
                </div>
            </CardContent>
        </Card>
    );
}
