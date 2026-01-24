"use client";

import { Card, CardHeader, CardTitle, CardContent } from '@/components/ui/card';
import { ParkingSquareIcon } from 'lucide-react';

export function ProjectInfoSection() {
    return (
        <Card className="h-full shadow-md hover:shadow-lg transition-shadow">
            <CardHeader>
                <CardTitle className="flex items-center text-2xl gap-2">
                    <ParkingSquareIcon className="h-6 w-6 text-blue-500" />
                    About the project
                </CardTitle>
            </CardHeader>
            <CardContent className="space-y-4">
                <div className="space-y-3">
                    <p className="text-sm text-gray-700 dark:text-gray-300">
                        An automated parking management system for small-sized vehicles (toy cars, RC vehicles) 
                        developed for the Embedded Software for the Internet of Things course.
                    </p>
                </div>

                <div className="space-y-3">
                    <h4 className="font-semibold text-base">Key Features</h4>
                    <ul className="text-sm text-gray-700 dark:text-gray-300 space-y-2">
                        <li className="flex gap-2">
                            <span className="text-blue-500">•</span>
                            <span><strong>RGB LED Indicator:</strong> Green for available spaces, red when full, blue for non-cars</span>
                        </li>
                        <li className="flex gap-2">
                            <span className="text-blue-500">•</span>
                            <span><strong>Automated Barriers:</strong> Entry/exit barriers lift automatically for authorized vehicles</span>
                        </li>
                        <li className="flex gap-2">
                            <span className="text-blue-500">•</span>
                            <span><strong>Smart Sensors:</strong> Weight sensor for vehicle discrimination, ultrasonic for exit detection</span>
                        </li>
                        <li className="flex gap-2">
                            <span className="text-blue-500">•</span>
                            <span><strong>Computer Vision:</strong> License plate recognition for vehicle tracking</span>
                        </li>
                        <li className="flex gap-2">
                            <span className="text-blue-500">•</span>
                            <span><strong>Real-time Dashboard:</strong> Monitor parking availability and vehicle count</span>
                        </li>
                        <li className="flex gap-2">
                            <span className="text-blue-500">•</span>
                            <span><strong>Access Control:</strong> Prevents heavy vehicles from entering without authorization</span>
                        </li>
                    </ul>
                </div>

                <div className="space-y-3">
                    <h4 className="font-semibold text-base">System Architecture</h4>
                    <p className="text-sm text-gray-700 dark:text-gray-300">
                        The system uses a finite state machine (FSM) to manage behavior, with non-blocking task processing 
                        for sensors, computer vision, and display updates. Features low-power sleep mode when idle, 
                        activating vision processing only when vehicles are detected.
                    </p>
                </div>

                <div className="space-y-3">
                    <h4 className="font-semibold text-base">Technology Stack</h4>
                    <p className="text-sm text-gray-700 dark:text-gray-300">
                        <strong>Embedded:</strong> ESP32-S3 with ESP-IDF framework, integrated camera, and multiple sensors
                    </p>
                    <p className="text-sm text-gray-700 dark:text-gray-300">
                        <strong>Web Services:</strong> Node.js backend with Express, Next.js frontend with React
                    </p>
                </div>

                <div className="space-y-2 pt-2 border-t">
                    <p className="text-xs text-gray-600 dark:text-gray-400">
                        <strong>Course:</strong> Embedded Software for the Internet of Things
                    </p>
                    <p className="text-xs text-gray-600 dark:text-gray-400">
                        <strong>Developed by:</strong> Mirco Stelzer, Leonardo Conforti, Daniel Casagranda, Amar Sidkir
                    </p>
                </div>
            </CardContent>
        </Card>
    );
}
