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
                        Tiny Parking System is an automated parking management solution designed for small-sized vehicles (toy cars, RC vehicles, and similar). It creates a realistic parking experience scaled down for educational and prototyping purposes, combining embedded systems, computer vision, and cloud services to autonomously manage vehicle entry, authentication, and parking space allocation.
                    </p>
                </div>

                <div className="space-y-1.5">
                    <h4 className="font-semibold text-[1.35rem]">
                        What it does and how it works
                    </h4>
                    <p className="text-sm text-gray-700">
                        The system operates through a sophisticated Finite State Machine that orchestrates multiple sensors and services. When a vehicle approaches, a weight sensor first discriminates between authorized cars and other objects (pedestrians, trucks). Upon detection, a camera captures the license plate, which is processed through a cloud-based computer vision service to verify against an allowed plates database. If authorized and parking spaces are available, the barrier opens to grant access. Throughout the process, the system maintains real-time synchronization with a web dashboard and cloud API for monitoring and control.
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
                                You are currently here: This section provides an overview of the project, including its purpose, architecture, and links to all documentation and resources including the presentation and demonstration video.
                            </p>
                        </div>

                        <div className="mt-4 space-y-1">
                            <h4 className="flex items-center gap-1 font-semibold text-lg">
                                <ActivityIcon className="text-green-500 size-5" />
                                System status
                            </h4>
                            <p className="text-sm text-gray-700">
                                Real-time monitoring of all active hardware modules and their operational status. Displays the status of the weight sensor, ultrasonic sensor, camera, servo motor, OLED display, and WiFi connectivity. Shows current error codes and diagnostic information for each module, updated continuously as the system operates and initializes.
                            </p>
                        </div>

                        <div className="mt-4 space-y-1">
                            <h4 className="flex items-center gap-1 font-semibold text-lg">
                                <FileTextIcon className="text-violet-500 size-5" />
                                System log
                            </h4>
                            <p className="text-sm text-gray-700">
                                Comprehensive archive of all recorded system events, warnings, and activities presented in chronological order. Includes vehicle entry and exit attempts, license plate recognition results with captured images, authorization decisions, and system initialization events. Fully searchable and sortable for easy investigation of historical data.
                            </p>
                        </div>

                        <div className="mt-4 space-y-1">
                            <h4 className="flex items-center gap-1 font-semibold text-lg">
                                <ParkingMeterIcon className="text-amber-500 size-5" />
                                Parking simulation
                            </h4>
                            <p className="text-sm text-gray-700">
                                Interactive 3D representation of the parking infrastructure with a real-time grid view showing occupied and available spots. Displays license plates of parked vehicles with animated transitions when vehicles enter or exit. Includes management controls to add or remove allowed license plates from the system whitelist directly from the dashboard.
                            </p>
                        </div>
                    </div>
                </div>

                <div className="space-y-3 pt-6 border-t">
                    <div className="space-y-1">
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
                </div>
            </CardContent>
        </Card>
    );
}