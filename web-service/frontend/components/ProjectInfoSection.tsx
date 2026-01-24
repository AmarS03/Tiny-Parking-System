"use client";

import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { ParkingSquareIcon, TextSearchIcon } from 'lucide-react';

export function ProjectInfoSection() {
    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow">
            <CardHeader>
                <CardTitle className="flex items-center text-xl gap-2">
                    <ParkingSquareIcon className="h-6 w-6 text-blue-500" />
                    About the project
                </CardTitle>
            </CardHeader>
            <CardContent>
                Info here
            </CardContent>
        </Card>
    );
}
