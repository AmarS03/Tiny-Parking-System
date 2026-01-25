"use client";

import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { AlertCircle, Info, FileText, CheckCircle2 } from 'lucide-react';

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

export function SystemLogSection(
    {
        logsData
    } : {
        logsData: logMessage[]
    }
) {
    const getIcon = (type: logMessage['type']) => {
        switch (type) {
            case 'success': return <CheckCircle2 className="h-4 w-4 text-green-500" />;
            case 'warning': return <AlertCircle className="h-4 w-4 text-orange-500" />;
            case 'error': return <AlertCircle className="h-4 w-4 text-red-500" />;
            default: return <Info className="h-4 w-4 text-violet-500" />;
        }
    };

    const getTypeTitle = (type: logMessage['type']) => {
        switch (type) {
            case 'success': return <span className="font-medium text-green-500">Success</span>;
            case 'warning': return <span className="font-medium text-orange-500">Warning</span>;
            case 'error': return <span className="font-medium text-red-500">Error</span>;
            default: return <span className="font-medium text-violet-500">Info</span>;
        }
    };

    return (
        <Card className="h-full shadow-md flex flex-col">
            <CardHeader className="pb-0">
                <CardTitle className="flex items-center text-2xl gap-2">
                    <FileText className="h-6 w-6 text-violet-500" />
                    System log
                </CardTitle>
            </CardHeader>
            <CardContent className="py-0">
                <div className="space-y-4">
                    {
                        logsData.length === 0 && (
                            <div className="text-center text-sm text-muted-foreground py-8">
                                No activity recorded yet. Start the ESP model to see the incoming updates!
                            </div>
                        )
                    }
                    {
                        [...logsData].reverse().map((log, index) => (
                            <div key={index} className="flex text-sm border-b pb-2 gap-8 last:border-0">
                                <div className='flex flex-col'>
                                    <div className='flex flex-row gap-1 items-center'>
                                        {getIcon(log.type)}
                                        {getTypeTitle(log.type)}
                                    </div>
                                    <span className="text-xs text-muted-foreground font-mono whitespace-nowrap">
                                        {formatDate(log.timestamp)}
                                    </span>
                                </div>
                                <div className='flex flex-col items-start gap-2'>
                                    <span className="text-foreground/90 text-lg font-normal mt-0.5">
                                        {log.message}
                                    </span>
                                    {log.imageUrl && (
                                        <img 
                                            src={log.imageUrl} 
                                            alt="Log preview" 
                                            className="h-64 object-contain"
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
