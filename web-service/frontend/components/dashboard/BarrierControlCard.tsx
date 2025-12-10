"use client";

import React from 'react';
import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { Switch } from '@/components/ui/switch';
import { Badge } from '@/components/ui/badge';
import { ShieldCheck, ShieldAlert, Lock, Unlock } from 'lucide-react';
import { useDashboard } from '@/components/dashboard-provider';

export function BarrierControlCard() {
    const { isBarrierOpen, toggleBarrier } = useDashboard();

    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow bg-gradient-to-br from-background to-accent/20">
            <CardHeader>
                <CardTitle className="flex items-center gap-2">
                    {isBarrierOpen ? <Unlock className="h-6 w-6 text-orange-500" /> : <Lock className="h-6 w-6 text-slate-500" />}
                    Barrier Control
                </CardTitle>
            </CardHeader>
            <CardContent className="flex flex-col items-center justify-center p-6 space-y-6">

                <div className={`p-6 rounded-full border-4 transition-all duration-500 ${isBarrierOpen ? 'border-orange-400 bg-orange-100 dark:bg-orange-950' : 'border-slate-200 bg-slate-50 dark:bg-slate-900'}`}>
                    {isBarrierOpen ? (
                        <ShieldAlert className="w-16 h-16 text-orange-500 animate-pulse" />
                    ) : (
                        <ShieldCheck className="w-16 h-16 text-slate-400" />
                    )}
                </div>

                <div className="flex flex-col items-center gap-3 w-full">
                    <div className="flex items-center justify-between w-full max-w-xs px-4 py-3 bg-card border rounded-xl shadow-sm">
                        <span className="font-semibold text-sm">Main Gate Status</span>
                        <Badge variant={isBarrierOpen ? "destructive" : "outline"} className={isBarrierOpen ? "bg-orange-500" : ""}>
                            {isBarrierOpen ? 'OPEN' : 'CLOSED'}
                        </Badge>
                    </div>

                    <div className="flex items-center gap-4 mt-2">
                        <span className={`text-sm font-medium transition-colors ${!isBarrierOpen ? "text-primary" : "text-muted-foreground"}`}>Close</span>
                        <Switch
                            checked={isBarrierOpen}
                            onCheckedChange={toggleBarrier}
                            className="data-[state=checked]:bg-orange-500 scale-125"
                        />
                        <span className={`text-sm font-medium transition-colors ${isBarrierOpen ? "text-orange-600 dark:text-orange-400" : "text-muted-foreground"}`}>Open</span>
                    </div>
                </div>

                <p className="text-xs text-muted-foreground text-center max-w-[200px]">
                    {isBarrierOpen
                        ? "Warning: Barrier is currently raised. Unauthorized entry possible."
                        : "System Secure: Barrier is lowered and locked."}
                </p>

            </CardContent>
        </Card>
    );
}
