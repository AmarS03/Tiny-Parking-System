"use client";

import { useState, useEffect } from "react";

import { ProjectInfoSection } from "@/components/ProjectInfoSection";
import { sensorStatus, SystemStatusSection } from "@/components/SystemStatusSection";
import { logMessage, SystemLogSection } from "@/components/SystemLogSection";
import { ParkingLotSection, parkingSpot } from "@/components/ParkingLotSection";

import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs";
import { ActivityIcon, FileTextIcon, ParkingMeterIcon, ParkingSquareIcon } from "lucide-react";

type apiData = {
    boardStatus: sensorStatus[];
	logs: logMessage[];
	allowedPlates: string[];
	spots: parkingSpot[];
	openingTime: string;
	closingTime: string;
	initializedAt: string;
	lastUpdatedAt: string;
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

function useMediaQuery(query: string) {
    const [matches, setMatches] = useState(false);

    useEffect(() => {
        const media = window.matchMedia(query);
        if (media.matches !== matches) {
            setMatches(media.matches);
        }
        const listener = () => setMatches(media.matches);
        media.addEventListener('change', listener);
        return () => media.removeEventListener('change', listener);
    }, [matches, query]);

    return matches;
}

export default function Home() {
    const isSmallScreen = useMediaQuery('(max-width: 768px)');
	
	const [data, updateData] = useState<apiData>(
		{
			boardStatus: [
				{ name: 'API', status: 'No connection', message: 'Unable to connect to API' },
			],
			logs: [],
			allowedPlates: [],
			spots: Array.from({ length: 10 }, (_, i) => ({
				id: i + 1,
				isOccupied: false,
			})),
			openingTime: '08:00',
			closingTime: '22:00',
			initializedAt: '',
			lastUpdatedAt: new Date().toISOString(),
		}
	);
	const [lastUpdated, setLastUpdated] = useState<string>('');
	const [timeDifference, setTimeDifference] = useState<string>('Awaiting data...');

	useEffect(() => {
		const fetchStatus = async () => {
			try {
				const response = await fetch('http://localhost:5000/status');
				const responseData = await response.json();

				updateData(responseData || {});
				setLastUpdated(new Date(responseData.lastUpdatedAt).toLocaleString('it-IT'));
				setTimeDifference(getTimeDifference(responseData.lastUpdatedAt));
			} catch (error) {
				console.error('Failed to fetch status:', error);
				setLastUpdated(new Date().toLocaleString('it-IT'));
				setTimeDifference('Unable to connect');
			}
		};

		fetchStatus();
		const interval = setInterval(fetchStatus, 2500); // Refreshes every 2.5 seconds
		return () => clearInterval(interval);
	}, []);

    return (
		<div className="min-h-screen flex flex-col items-center w-full p-4 sm:p-6 lg:p-8">
			<div className="w-full max-w-[1600px]">
				<header className="mb-3 flex flex-row justify-between items-center gap-4">
					<div>
						<h1 className="text-4xl font-bold mb-1 tracking-tight text-slate-900">
							🅿️ Tiny Parking System Dashboard
						</h1>
						<p className="text-lg text-muted-foreground">
							Our project for the Embedded Software for the Internet of Things course - 2025/2026
						</p>
					</div>
				</header>

				<main>
					<Tabs defaultValue="info" orientation={isSmallScreen ? "vertical" : "horizontal"}>
						<TabsList className="mb-0.5">
							<TabsTrigger value="info">
								<ParkingSquareIcon className="text-blue-500" />
								Project info
							</TabsTrigger>
							<TabsTrigger value="status">
								<ActivityIcon className="text-green-500" />
								System status
							</TabsTrigger>
							<TabsTrigger value="log">
								<FileTextIcon className="text-violet-500" />
								System log
							</TabsTrigger>
							<TabsTrigger value="parking">
								<ParkingMeterIcon className="text-amber-500" />
								Parking simulation
							</TabsTrigger>
						</TabsList>
						<TabsContent value="info">
							<ProjectInfoSection />
						</TabsContent>
						<TabsContent value="status">
							<SystemStatusSection sensorData={data.boardStatus} lastUpdated={lastUpdated} timeDifference={timeDifference} />
						</TabsContent>
						<TabsContent value="log">
							<SystemLogSection logsData={data.logs} />
						</TabsContent>
						<TabsContent value="parking">
							<ParkingLotSection spotsData={data.spots} allowedPlates={data.allowedPlates} />
						</TabsContent>
					</Tabs>
				</main>
			</div>
		</div>
	);
}
