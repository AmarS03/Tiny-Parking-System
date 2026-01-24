"use client";

import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { AlertCircle, Info, FileText, CheckCircle2 } from 'lucide-react';
import { useEffect, useState } from 'react';

export interface logMessage {
    timestamp: string;
    message: string;
    imageUrl?: string;
    type: 'info' | 'success' | 'warning' | 'error';
};

const formatDate = (dateString: string) => {
    const date = new Date(dateString);
    const day = String(date.getDate()).padStart(2, '0');
    const month = String(date.getMonth() + 1).padStart(2, '0');
    const year = date.getFullYear();
    const time = date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });

    return `${day}/${month}/${year} - ${time}`;
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

export function SystemLogSection() {
    const [logs, setLogs] = useState<logMessage[]>([]);
    const [lastUpdated, setLastUpdated] = useState<string>('');
    const [timeDifference, setTimeDifference] = useState<string>('Awaiting data...');

    useEffect(() => {
        // Fetch initial logs from API
        const fetchInitialLogs = async () => {
            try {
                const response = await fetch('https://tinyparkingsystem-api.vercel.app/status');
                const data = await response.json();
                setLogs(data.logs || []);
                setLastUpdated(new Date(data.lastUpdatedAt).toLocaleString('it-IT'));
                setTimeDifference(getTimeDifference(data.lastUpdatedAt));
            } catch (error) {
                console.error('Failed to fetch initial logs:', error);
                setLogs([
                    { timestamp: new Date().toISOString(), message: 'Unable to connect to API', type: 'error' },
                ]);
            }
        };

        fetchInitialLogs();

        // Connect to SSE stream for real-time updates
        const eventSource = new EventSource('https://tinyparkingsystem-api.vercel.app/status/logs/stream');
        
        eventSource.onmessage = (event) => {
            try {
                const newLog = JSON.parse(event.data) as logMessage;
                setLogs(prevLogs => [...prevLogs, newLog]);
                setLastUpdated(new Date(newLog.timestamp).toLocaleString('it-IT'));
                setTimeDifference(getTimeDifference(newLog.timestamp));
            } catch (error) {
                console.error('Failed to parse SSE message:', error);
            }
        };

        eventSource.onerror = () => {
            console.error('SSE connection error');
            eventSource.close();
        };

        // Update time difference every minute
        const interval = setInterval(() => {
            if (logs.length > 0) {
                setTimeDifference(getTimeDifference(logs[logs.length - 1].timestamp));
            }
        }, 60000);

        return () => {
            eventSource.close();
            clearInterval(interval);
        };
    }, []);

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
                                        <img 
                                            src={log.imageUrl} 
                                            alt="Log preview" 
                                            className="h-64"
                                            crossOrigin="anonymous"
                                        />
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
