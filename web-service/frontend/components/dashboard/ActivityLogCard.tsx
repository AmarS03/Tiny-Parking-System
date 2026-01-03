"use client";

import React from 'react';
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { ScrollArea } from '@/components/ui/scroll-area';
import { AlertCircle, Info, FileText, CheckCircle2 } from 'lucide-react';
import { useDashboard } from '@/components/dashboard-provider';

export interface ActivityLog {
    id: string;
    timestamp: string;
    message: string;
    type: 'info' | 'success' | 'warning' | 'error';
}

// Helper to format date
const formatDate = (dateString: string) => {
    return new Date(dateString).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
};

export function ActivityLogCard() {
    // We will read a shared key "activity_log"
    // Note: Writing to this log from OTHER components requires them to use the same hook
    const { logs } = useDashboard();

    const getIcon = (type: ActivityLog['type']) => {
        switch (type) {
            case 'success': return <CheckCircle2 className="h-4 w-4 text-green-500" />;
            case 'warning': return <AlertCircle className="h-4 w-4 text-orange-500" />;
            case 'error': return <AlertCircle className="h-4 w-4 text-red-500" />;
            default: return <Info className="h-4 w-4 text-blue-500" />;
        }
    };

    return (
        <Card className="h-full shadow-md flex flex-col">
            <CardHeader className="pb-3">
                <CardTitle className="flex items-center gap-2 text-base">
                    <FileText className="h-5 w-5 text-slate-500" />
                    System Activity Log
                </CardTitle>
            </CardHeader>
            <CardContent className="flex-1 p-0">
                <ScrollArea className="h-[250px] px-6">
                    <div className="space-y-4 pb-4">
                        {logs.length === 0 && (
                            <div className="text-center text-sm text-muted-foreground py-8">
                                No activity recorded yet.
                            </div>
                        )}
                        {[...logs].reverse().map((log) => (
                            <div key={log.id} className="flex gap-3 items-start text-sm border-b pb-3 last:border-0">
                                <span className="text-xs text-muted-foreground font-mono mt-0.5 whitespace-nowrap">
                                    {formatDate(log.timestamp)}
                                </span>
                                <div className="mt-0.5">{getIcon(log.type)}</div>
                                <span className="text-foreground/90 leading-tight">
                                    {log.message}
                                </span>
                            </div>
                        ))}
                    </div>
                </ScrollArea>
            </CardContent>
        </Card>
    );
}
