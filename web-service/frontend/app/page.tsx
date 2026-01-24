import { SystemLogSection } from "@/components/SystemLogSection";
import { ProjectInfoSection } from "@/components/ProjectInfoSection";
import { SystemStatusSection } from "@/components/SystemStatusSection"

import { Tabs, TabsContent, TabsList, TabsTrigger } from "@/components/ui/tabs"
import { Activity, ActivityIcon, FileText, SquareParkingIcon, TextSearchIcon } from "lucide-react"
import { Card, CardContent, CardDescription, CardHeader, CardTitle } from "@/components/ui/card";

export default function Home() {
    return (
            <div className="min-h-screen bg-slate-50 dark:bg-slate-950 p-4 sm:p-6 lg:p-8 font-(family-name:--font-geist-sans)">
                <header className="mb-4 flex flex-row justify-between items-center gap-4">
                    <div>
                        <h1 className="text-4xl font-bold mb-1 tracking-tight text-slate-900 dark:text-slate-100">
                            🅿️ Tiny Parking System Dashboard
                        </h1>
                        <p className="text-lg text-muted-foreground">
                            Our project for the Embedded Software for the Internet of Things course - 2025/2026
                        </p>
                    </div>
                </header>

                <main>
					<Tabs defaultValue="info">
						<TabsList>
							<TabsTrigger value="info">
								<TextSearchIcon />
								About the project
							</TabsTrigger>
							<TabsTrigger value="status">
								<ActivityIcon className="text-green-500" />
								System status
							</TabsTrigger>
							<TabsTrigger value="log">
								<FileText className="text-violet-500" />
								System log
							</TabsTrigger>
							<TabsTrigger value="parking">
								<SquareParkingIcon className="text-blue-500" />
								Parking visualization
							</TabsTrigger>
						</TabsList>
						<TabsContent value="info">
                            <ProjectInfoSection />
						</TabsContent>
						<TabsContent value="status">
                            <SystemStatusSection />
						</TabsContent>
						<TabsContent value="log">
							<SystemLogSection />
						</TabsContent>
						<TabsContent value="parking">
							<Card className="h-full shadow-md hover:shadow-lg transition-shadow">
								<CardHeader>
									<CardTitle className="flex items-center text-xl gap-2">
										<SquareParkingIcon className="h-6 w-6 text-blue-500" />
										Parking visualization
									</CardTitle>
									<CardDescription className='text-muted-foreground'>
										<span className='text-black font-medium'>Last updated:</span> 26/01/2026 - 14:23
										<br />
										<span className='text-xs'>(over 2 hours ago)</span>
									</CardDescription>
								</CardHeader>
								<CardContent>
									Parking visualization here
								</CardContent>
							</Card>
						</TabsContent>
					</Tabs>
                </main>
            </div>
    );
}
