"use client";

import { Card, CardHeader, CardTitle, CardContent, CardDescription } from '@/components/ui/card';
import { ScrollArea } from '@/components/ui/scroll-area';
import { AlertCircle, Info, FileText, CheckCircle2 } from 'lucide-react';

export interface logMessage {
    timestamp: string;
    message: string;
    imageUrl?: string;
    type: 'info' | 'success' | 'warning' | 'error';
}

const formatDate = (dateString: string) => {
    const date = new Date(dateString);
    const day = String(date.getDate()).padStart(2, '0');
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const year = date.getFullYear();
    const time = date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });

    return `${day}/${month}/${year} - ${time}`;
};

export function SystemLogSection() {
    // We will read a shared key "activity_log"
    // Note: Writing to this log from OTHER components requires them to use the same hook
    const logs: logMessage[] = [
        { timestamp: new Date().toISOString(), message: "System initialized.", type: 'info' },
        { timestamp: new Date().toISOString(), message: "User logged in.", type: 'success' },
        { timestamp: new Date().toISOString(), message: "Low battery.", type: 'warning' },
        { timestamp: new Date().toISOString(), message: "Unable to connect to server.", type: 'error' },
        { timestamp: new Date().toISOString(), message: "Unable to initialize.", type: 'error' },
        { 
            timestamp: new Date().toISOString(), 
            message: "Vehicle entering detected. License Plate: ABC-1234", 
            imageUrl: "https://www.circuitdigest.cloud/static/anpr_uploads/anpr_142e06c40989_20260122_161010_032.jpeg", 
            type: 'info' 
        }
    ];

    const getIcon = (type: logMessage['type']) => {
        switch (type) {
            case 'success': return <CheckCircle2 className="h-4 w-4 text-green-500" />;
            case 'warning': return <AlertCircle className="h-4 w-4 text-orange-500" />;
            case 'error': return <AlertCircle className="h-4 w-4 text-red-500" />;
            default: return <Info className="h-4 w-4 text-blue-500" />;
        }
    };

    const getTypeTitle = (type: logMessage['type']) => {
        switch (type) {
            case 'success': return <span className="font-medium text-green-500">Success</span>;
            case 'warning': return <span className="font-medium text-orange-500">Warning</span>;
            case 'error': return <span className="font-medium text-red-500">Error</span>;
            default: return <span className="font-medium text-blue-500">Info</span>;
        }
    };

    return (
        <Card className="h-full shadow-md flex flex-col">
            <CardHeader className="pb-0">
                <CardTitle className="flex items-center text-xl gap-2">
                    <FileText className="h-6 w-6 text-violet-500" />
                    System log
                </CardTitle>
            </CardHeader>
            <CardContent className="py-0">
                <div className="space-y-4">
                    {
                        logs.length === 0 && (
                            <div className="text-center text-sm text-muted-foreground py-8">
                                No activity recorded yet.
                            </div>
                        )
                    }
                    {
                        [...logs].reverse().map((log, index) => (
                            <div key={index} className="flex gap-8 items-start text-sm border-b pb-2 last:border-0">
                                <div className='flex flex-col'>
                                    <div className='flex flex-row gap-1 items-center'>
                                        {getIcon(log.type)}
                                        {getTypeTitle(log.type)}
                                    </div>
                                    <span className="text-xs text-muted-foreground font-mono whitespace-nowrap">
                                        {formatDate(log.timestamp)}
                                    </span>
                                </div>
                                <div className='flex flex-col gap-2'>
                                    <span className="text-foreground/90 text-lg font-normal mt-0.5">
                                        {log.message}
                                    </span>
                                    {log.imageUrl && (
                                        <img src={log.imageUrl} alt="Log preview" className="h-64" />
                                    )}
                                </div>
                            </div>
                        ))
                    }
                </div>
            </CardContent>
        </Card>
    );
}
