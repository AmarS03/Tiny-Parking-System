"use client";

import React, { useEffect } from 'react';
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { Badge } from '@/components/ui/badge';
import { Scale, Video, Activity, Signal } from 'lucide-react';
import { useDashboard } from '@/components/dashboard-provider';

export interface Sensor {
    id: string;
    name: string;
    type: 'weight' | 'camera' | 'ultrasonic' | 'generic';
    status: 'active' | 'inactive';
}

export function SensorStatusCard() {
    const { sensors } = useDashboard();

    const getIcon = (type: Sensor['type']) => {
        switch (type) {
            case 'weight': return <Scale className="h-5 w-5" />;
            case 'camera': return <Video className="h-5 w-5" />;
            case 'ultrasonic': return <Signal className="h-5 w-5" />;
            default: return <Activity className="h-5 w-5" />;
        }
    };

    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow">
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    <Activity className="h-6 w-6 text-blue-500" />
                    Sensor Status
                </CardTitle>
            </CardHeader>
            <CardContent>
                <div className="space-y-4">
                    {sensors.map((sensor) => (
                        <div key={sensor.id} className="flex items-center justify-between p-3 rounded-lg border bg-card/50 hover:bg-accent/50 transition-colors">
                            <div className="flex items-center gap-3">
                                <div className="p-2 bg-background rounded-full border shadow-sm text-foreground/80">
                                    {getIcon(sensor.type)}
                                </div>
                                <span className="font-medium text-sm sm:text-base">{sensor.name}</span>
                            </div>

                            <Badge
                                variant={sensor.status === 'active' ? "default" : "destructive"}
                                className={sensor.status === 'active' ? "bg-green-500 hover:bg-green-600" : ""}
                            >
                                {sensor.status === 'active' ? 'Active' : 'Inactive'}
                            </Badge>
                        </div>
                    ))}
                </div>
            </CardContent>
        </Card>
    );
}
