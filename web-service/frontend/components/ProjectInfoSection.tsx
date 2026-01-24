"use client";

import { Card, CardHeader, CardTitle, CardContent, CardDescription } from '@/components/ui/card';
import { ActivityIcon, FileTextIcon, ParkingMeterIcon, ParkingSquareIcon } from 'lucide-react';

export function ProjectInfoSection() {
    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow">
            <CardHeader>
                <CardTitle className="flex items-center text-2xl gap-2">
                    <ParkingSquareIcon className="h-6 w-6 text-blue-500" />
                    Project info
                </CardTitle>
                <CardDescription className='text-muted-foreground'>
                    <a href="https://github.com/AmarS03/Tiny-Parking-System/blob/main/README.md" target="_blank" className="link">
                        Click here to visit the Github repository README.md
                    </a>
                </CardDescription>
            </CardHeader>
            <CardContent className="space-y-8 pt-2">
                <div className="space-y-1.5">
                    <h4 className="font-semibold text-[1.35rem]">
                        About this project
                    </h4>
                    <p className="text-sm text-gray-700">
                        Text here
                    </p>
                </div>

                <div className="space-y-1.5">
                    <h4 className="font-semibold text-[1.35rem]">
                        What it does and how it works
                    </h4>
                    <p className="text-sm text-gray-700">
                        Text here
                    </p>
                </div>

                <div>
                    <h4 className="font-semibold text-[1.35rem]">
                        Dashboard sections
                    </h4>
                    <div className="space-y-5 mt-4">
                        <div className="space-y-1">
                            <h4 className="flex items-center gap-1 font-semibold text-lg">
                                <ParkingSquareIcon className="text-blue-500 size-5" />
                                Project info
                            </h4>
                            <p className="text-sm text-gray-700">
                                You are here :)
                            </p>
                        </div>

                        <div className="mt-4 space-y-1">
                            <h4 className="flex items-center gap-1 font-semibold text-lg">
                                <ActivityIcon className="text-green-500 size-5" />
                                System status
                            </h4>
                            <p className="text-sm text-gray-700">
                                Text here
                            </p>
                        </div>

                        <div className="mt-4 space-y-1">
                            <h4 className="flex items-center gap-1 font-semibold text-lg">
                                <FileTextIcon className="text-violet-500 size-5" />
                                System log
                            </h4>
                            <p className="text-sm text-gray-700">
                                Text here
                            </p>
                        </div>

                        <div className="mt-4 space-y-1">
                            <h4 className="flex items-center gap-1 font-semibold text-lg">
                                <ParkingMeterIcon className="text-amber-500 size-5" />
                                Parking simulation
                            </h4>
                            <p className="text-sm text-gray-700">
                                Text here
                            </p>
                        </div>
                    </div>
                </div>

                <div className="space-y-2 pt-2 border-t">
                    <p className="text-xs text-gray-600 dark:text-gray-400">
                        <strong>Course:</strong> Embedded Software for the Internet of Things
                    </p>
                    <p className="text-xs text-gray-600 dark:text-gray-400">
                        <strong>Taught by:</strong> prof. Kasim Sinan Yildirim, during the 2025/2026 academic year
                    </p>
                    <p className="text-xs text-gray-600 dark:text-gray-400">
                        <strong>Team members:</strong> Mirco Stelzer, Leonardo Conforti, Daniel Casagranda and Amar Sidkir
                    </p>
                </div>
            </CardContent>
        </Card>
    );
}
