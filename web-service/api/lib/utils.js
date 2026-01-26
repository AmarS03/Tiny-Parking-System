export function isLicensePlateValid(licensePlate) {
	const plateRegex = /^[A-Z0-9]{7}$/;
	return plateRegex.test(licensePlate);
}